#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "jobs.h"

/* PID du job de premier plan (0 si aucun). Nous stockons le PID du
 * processus principal du job (le leader) afin de pouvoir rediriger les
 * signaux (ex : SIGINT) vers le groupe de processus du job via kill(-pid,...).
 */
static pid_t fg_pid = 0; /* job de premier plan en cours, 0 sinon */

/* PID du job en arrière-plan (on n'autorise qu'un seul job BG dans ce TP).
 * Les terminaisons de ce job sont détectées par SIGCHLD et récoltées pour
 * éviter que le processus devienne zombie.
 */
static pid_t bg_pid = 0; /* job en arrière-plan, 0 sinon */

/*
 * Handler SIGCHLD
 * ----------------
 * Ce handler est installé avec SA_SIGINFO : il reçoit des informations
 * sur le child via le paramètre `info` (notamment si_pid). Ici, on se
 * contente de récolter la terminaison du job en arrière-plan (bg_pid)
 * pour afficher son code de sortie et éviter les zombies.
 *
 * Remarque : le job de premier plan est attendu explicitement par
 * waitpid(..., 0) dans la fonction jobs_run_foreground, il ne faut donc
 * pas que le handler réapprenne ce child (sinon waitpid bloquant échouerait).
 */
static void sigchld_handler(int signum, siginfo_t *info, void *ucontext) {
    (void)signum;
    (void)info;
    (void)ucontext;

    int status;
    pid_t pid = info->si_pid;

    if (pid <= 0) return;

    /* On ne récolte que le job en arrière-plan ici */
    if (pid == bg_pid) {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r > 0) {
            bg_pid = 0;
            if (WIFEXITED(status)) {
                printf("Background job exited with code %d\n", WEXITSTATUS(status));
            } else {
                printf("Background job exited\n");
            }
            fflush(stdout);
        }
    }
}

/*
 * SIGINT (Ctrl+C)
 * ---------------
 * Ce handler redirige SIGINT vers le job de premier plan en envoyant
 * le signal au groupe de processus: `kill(-fg_pid, SIGINT)`. Ainsi, si
 * le job lance plusieurs processus (pipeline), tous recevront SIGINT.
 */
static void sigint_handler(int signum) {
    (void)signum;
    if (fg_pid > 0) {
        /* Send to the foreground process group so that any children also receive it */
        kill(-fg_pid, SIGINT);
    }
}

/*
 * SIGHUP
 * ------
 * Lorsqu'une session terminale est fermée, SIGHUP est envoyé : le shell
 * relaie SIGHUP aux jobs connus (foreground et background) en ciblant
 * leurs groupes de processus. Le shell quitte ensuite.
 *
 * Note : on évite d'attendre (bloquer) dans un handler ; l'envoi du signal
 * suffit normalement pour provoquer la terminaison des children (ex : sleep).
 */
static void sighup_handler(int signum) {
    (void)signum;
    /* Envoyer SIGHUP aux process groups des jobs connus (si définis) */
    if (fg_pid > 0) kill(-fg_pid, SIGHUP);
    if (bg_pid > 0) kill(-bg_pid, SIGHUP);

    /* Exit immediately after signalling children; they should terminate on SIGHUP. */
    _exit(0);
}

void jobs_init(void) {
    struct sigaction sa;

    /* SIGCHLD : handler avec SA_SIGINFO */
    memset(&sa, 0, sizeof sa);
    sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP;
    sa.sa_sigaction = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    /* SIGINT : Ctrl+C → redirigé vers le job de premier plan */
    memset(&sa, 0, sizeof sa);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* SIGTERM et SIGQUIT : ignorés (comportement classique des shells) */
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    /* SIGHUP : tout arrêter proprement */
    memset(&sa, 0, sizeof sa);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sighup_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, NULL);
}

int jobs_run_foreground(char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        /* Processus enfant : on rétablit les handlers par défaut. */
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGHUP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        /* Put the child in its own process group so signals can target the job */
        if (setpgid(0, 0) < 0) {
            /* ignore errors here */
        }

        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }

    /* Ensure the child has its own process group (best-effort) */
    if (setpgid(pid, pid) < 0) {
        /* ignore errors */
    }

    /* Processus parent : attendre le job de premier plan */
    fg_pid = pid;
    int status;
    for (;;) {
        pid_t r = waitpid(pid, &status, 0);
        if (r < 0) {
            if (errno == EINTR) continue; /* interrompu par signal → on réessaye */
            perror("waitpid");
            fg_pid = 0;
            return -1;
        }
        break;
    }
    fg_pid = 0;

    if (WIFEXITED(status)) {
        printf("Foreground job exited with code %d\n", WEXITSTATUS(status));
    } else {
        printf("Foreground job exited\n");
    }
    fflush(stdout);
    return 0;
}

int jobs_run_background(char **argv) {
    if (bg_pid != 0) {
        fprintf(stderr, "Error: background job already running (pid %d)\n", bg_pid);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        /* Enfant : job en arrière-plan */

        /* Rétablir les handlers par défaut. */
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGHUP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        /* Put the background child in its own process group */
        if (setpgid(0, 0) < 0) {
            /* ignore errors */
        }

        /* Rediriger stdin vers /dev/null pour ne pas perturber le shell */
        int fd = open("/dev/null", O_RDONLY);
        if (fd >= 0) {
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }

    /* Parent : ne pas attendre, juste mémoriser le pid */
    bg_pid = pid;
    printf("Started background job with pid %d\n", pid);
    fflush(stdout);
    return 0;
}

void jobs_on_shell_exit(void) {
    if (bg_pid > 0) {
        kill(-bg_pid, SIGHUP);
    }
}

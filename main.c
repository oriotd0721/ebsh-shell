#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parse.h"
#include "builtin.h"
#include "jobs.h"

/*
 * main.c
 * ------
 * Boucle principale du shell :
 * 1) initialisation des handlers et structures de gestion des jobs (jobs_init)
 * 2) boucle de lecture/traitement : afficher le prompt, lire la ligne, parser
 * 3) si builtin → exécuter dans le processus du shell (cd, exit)
 * 4) sinon → exécuter un job foreground (attendre) ou background (ne pas attendre)
 * 5) lors de la sortie, prévenir les jobs en arrière-plan (jobs_on_shell_exit)
 *
 * Points à expliquer à l'oral :
 * - distinction job vs builtin : builtin s'exécute dans le shell (ex : cd)
 * - jobs foreground utilisent waitpid bloquant ; background sont réapérés via SIGCHLD
 */

int main(void) {
    char *line = NULL;
    size_t len = 0;

    jobs_init();

    for (;;) {
        /* Afficher un prompt simple */
        printf("ebsh> ");
        fflush(stdout);

        /* getline() alloue ou réalloue line si besoin */
        ssize_t r = getline(&line, &len, stdin);
        if (r < 0) {
            /* EOF (Ctrl+D) ou erreur → on quitte comme "exit" */
            printf("\n");
            break;
        }

        char **argv = NULL;
        int background = 0;
        int argc = parse_line(line, &argv, &background);
        if (argc < 0) {
            fprintf(stderr, "parse error\n");
            continue;
        }
        if (argc == 0) {
            /* ligne vide */
            free_argv(argv);
            continue;
        }

        /* Builtins (cd, exit) ne vont jamais en tâche de fond :
         * - cd doit changer le répertoire du shell lui‑même
         * - exit demande la sortie propre du shell (main s'en occupe)
         */
        if (is_builtin(argv[0])) {
            int quit = run_builtin(argv);
            free_argv(argv);
            if (quit == 1) {
                /* exit */
                break;
            }
            continue;
        }

        /* Sinon, c'est un job normal */
        if (background) {
            jobs_run_background(argv);
        } else {
            jobs_run_foreground(argv);
        }

        free_argv(argv);
    }

    jobs_on_shell_exit();
    free(line);
    return 0;
}

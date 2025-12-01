#ifndef JOBS_H
#define JOBS_H

/* Initialise la gestion des jobs et des signaux (SIGCHLD, SIGINT, SIGHUP, etc.) */
void jobs_init(void);

/* Lance un job de premier plan, attend sa fin, et affiche son code de sortie. */
int jobs_run_foreground(char **argv);

/* Lance un job en arrière-plan (un seul à la fois).
 * Si un job est déjà en arrière-plan, affiche un message d'erreur.
 */
int jobs_run_background(char **argv);

/* Appelé quand le shell va quitter (pour tuer éventuellement un job en arrière-plan). */
void jobs_on_shell_exit(void);

#endif

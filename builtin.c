#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "builtin.h"

/*
 * builtin.c
 * ---------
 * Implémentation simple des commandes internes au shell :
 * - `cd <dir>` : change le répertoire courant du processus shell (chdir)
 * - `exit` : demande la sortie propre du shell (main interprète le retour)
 *
 * Remarques :
 * - `cd` doit être un builtin (changer le répertoire dans le shell parent)
 * - on essaie d'utiliser realpath() pour canoniser le chemin, et on met
 *   `PWD` dans l'environnement afin d'aider certains outils d'inspection.
 */

int is_builtin(const char *cmd) {
    if (!cmd) return 0;
    return (strcmp(cmd, "cd") == 0) || (strcmp(cmd, "exit") == 0);
}

int run_builtin(char **argv) {
    if (!argv || !argv[0]) return -1;

    if (strcmp(argv[0], "cd") == 0) {
        /* cd <dir> */
        if (!argv[1]) {
            fprintf(stderr, "cd: missing operand\n");
            return 0;
        }
        /* Use realpath to change to the canonical path when possible. */
        char resolved[PATH_MAX];
        if (realpath(argv[1], resolved) != NULL) {
            if (chdir(resolved) < 0) {
                perror("cd");
            } else {
                setenv("PWD", argv[1], 1);
            }
        } else {
            /* realpath failed (e.g. non-existing) - fall back to chdir to
             * preserve the original behavior and report the error. */
            if (chdir(argv[1]) < 0) {
                perror("cd");
            } else {
                setenv("PWD", argv[1], 1);
            }
        }
        return 0;
    }

    if (strcmp(argv[0], "exit") == 0) {
        /* On demande au shell de quitter proprement */
        return 1;
    }

    return -1; /* ne devrait pas arriver si is_builtin est utilisé */
}

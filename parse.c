#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

/*
 * parse.c
 * -------
 * Fonctions de parsing très simples :
 * - tokenise la ligne de commande en utilisant strtok (séparateurs espace/tab)
 * - détecte si la commande doit être lancée en arrière-plan via un '&' final
 * - construit un tableau `argv` compatible avec execvp (terminé par NULL)
 *
 * Remarques pédagogiques :
 * - `strtok` modifie la chaîne d'entrée (elle doit être modifiable)
 * - ce parser n'implémente pas la gestion des quotes, redirections, pipes, etc.
 */

int parse_line(char *line, char ***argv_out, int *background) {
    *background = 0;
    *argv_out = NULL;

    if (!line) return -1;

    /* Retirer le '\n' éventuel en fin de ligne */
    char *nl = strchr(line, '\n');
    if (nl) *nl = '\0';

    size_t cap = 4;
    char **argv = malloc(cap * sizeof *argv);
    if (!argv) return -1;

    int argc = 0;
    char *tok = strtok(line, " \t");
    while (tok) {
        if ((size_t)(argc + 1) >= cap) {
            cap *= 2;
            char **tmp = realloc(argv, cap * sizeof *tmp);
            if (!tmp) {
                free(argv);
                return -1;
            }
            argv = tmp;
        }
        argv[argc++] = tok;
        tok = strtok(NULL, " \t");
    }

    if (argc == 0) {
        free(argv);
        return 0;
    }

    /* Gestion du '&' final pour les jobs en arrière-plan */
    if (strcmp(argv[argc - 1], "&") == 0) {
        *background = 1;
        argc--;           /* On enlève le '&' de argv */
        if (argc == 0) {  /* ligne qui ne contient que '&' → on ignore */
            free(argv);
            return 0;
        }
    }

    argv[argc] = NULL;
    *argv_out = argv;
    return argc;
}

void free_argv(char **argv) {
    /* Libère la mémoire allouée pour la table d'arguments. Les chaînes
     * elles-mêmes pointent dans la buffer initiale fournie à parse_line.
     */
    free(argv);
}

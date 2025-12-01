#ifndef PARSE_H
#define PARSE_H

/* Analyse une ligne de commande en argv.
 * - line : chaîne modifiable (sera tokenisée avec strtok)
 * - argv_out : tableau de char* (terminé par NULL)
 * - background : mis à 1 si la commande finit par '&', 0 sinon
 * Retourne le nombre d'arguments (argc) ou -1 en cas d'erreur.
 */
int parse_line(char *line, char ***argv_out, int *background);

/* Libère le tableau argv (mais PAS les chaînes, qui pointent dans line). */
void free_argv(char **argv);

#endif

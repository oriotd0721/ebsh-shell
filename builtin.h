#ifndef BUILTIN_H
#define BUILTIN_H

/* Retourne 1 si cmd est une builtin (cd, exit), 0 sinon */
int is_builtin(const char *cmd);

/* Exécute la builtin.
 * Retourne:
 *   1 → builtin exit → le shell doit se terminer
 *   0 → builtin exécutée, on continue
 *  -1 → erreur
 */
int run_builtin(char **argv);

#endif

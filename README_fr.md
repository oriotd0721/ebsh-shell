# ebsh — Mini-shell 

Projet universitaire réalisé par Oriance Oternaud, étudiante en Sciences Informatique à l’Université de Genève (UNIGE).

**Descriptifs**
- **Sujet :** ebsh — un mini-shell écrit en C pour l'enseignement des concepts de processus et gestion de jobs.
- **Langage :** C
- **Licence :** MIT
- **Build :** Make / gcc

Ce dépôt contient un mini-shell (prompt `ebsh>`) qui illustre : parsing simple de lignes de commande, exécution de commandes externes, builtins (`cd`, `exit`) et gestion basique de jobs (foreground / background) avec handlers de signaux.

**Fonctionnalités principales**
- Prompt simple `ebsh>` et boucle de lecture/évaluation.
- Builtins : `cd <dir>`, `exit`.
- Exécution de commandes externes via `fork()` + `execvp()`.
- Jobs : lancement en arrière-plan avec `&` final (un job BG supporté dans cette version), message de démarrage et notification à la terminaison via `SIGCHLD`.
- Gestion de `SIGINT` (Ctrl+C) redirigée vers le job de premier plan.
- Parser minimal : tokenisation par espaces/tabs, pas de gestion des quotes, pipes, ni redirections.

**Structure du projet**
```
├── main.c        # boucle principale du shell
├── parse.c       # parser minimal + free_argv
├── parse.h
├── builtin.c     # cd, exit
├── builtin.h
├── jobs.c        # gestion des jobs, signaux
├── jobs.h
├── Makefile      # (vide dans le dépôt) -> compilation manuelle possible
└── README.md
```

**Compilation**

Le `Makefile` fourni dans ce dépôt est vide : vous pouvez compiler manuellement avec `gcc` ou ajouter un Makefile simple.

- Vérifier que vous avez un compilateur C (`gcc`) :
  - `gcc --version`

- Compiler manuellement (depuis le répertoire du projet) :
```bash
gcc -std=gnu11 -Wall -Wextra -pedantic -o ebsh main.c parse.c builtin.c jobs.c
```

- Un `Makefile` minimal est fourni dans le dépôt. Utilisez :
	- `make` — construit l'exécutable `ebsh`.
	- `make clean` — supprime l'exécutable.


**Utilisation**

Lancer le shell en mode interactif :
```bash
./ebsh
```

Exemples de commandes à l'intérieur du shell :

- Commande externe (foreground) :
```
ebsh> pwd
```

- Builtin `cd` :
```
ebsh> cd ..
```

- Lancer en arrière-plan (background) :
```
ebsh> sleep 5 &
Started background job with pid 12345
```

- Le shell affiche la terminaison du job background via le handler SIGCHLD :
```
Background job exited with code 0
```

- Pour quitter :
```
ebsh> exit
```

Exemple non-interactif (script) :
```bash
printf "pwd\ncd ..\npwd\nsleep 1 &\necho after\nexit\n" | ./ebsh
```

**Contexte académique et objectifs pédagogiques**

Ce projet fait partie d'un travail de cours en Systèmes d'Exploitation (12X009) et a des objectifs pédagogiques clairement définis :

- Créer des processus avec la fonction `fork`.
- Gérer ces processus, notamment éviter les processus zombies (récolte avec `waitpid` / gestion de `SIGCHLD`).
- Gérer les signaux envoyés au shell (ex. rediriger `SIGINT` vers le job de premier plan, réagir à `SIGHUP`).

Le mini-shell illustre ces notions à travers une implémentation simple de parsing, d'exécution de commandes, de builtins (`cd`, `exit`) et de contrôle basique des jobs (foreground / background).

**Validation**

Pour vérifier le comportement des commandes externes, comparez la sortie de programmes standards dans votre shell habituel (`/bin/sh` ou `bash`), par exemple `pwd`, `sleep`, etc. Les codes de sortie des processus foreground sont affichés par le shell sous la forme :

```
Foreground job exited with code 0
```

**Outils & technologies**
- Langage : C
- Compilateur : gcc
- OS de développement : macOS (aussi testé sur Linux)
- Contrôle de version : Git & GitHub

**Licence**
Projet publié sous licence **MIT**. Voir le fichier `LICENSE` pour le texte complet.


© 2025 Oriance Oternaud — University of Geneva (UNIGE)

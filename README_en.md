# ebsh — Mini-shell

University coursework project by Oriance Oternaud — Computer Science student at the University of Geneva (UNIGE).

**Description**
- **Topic:** ebsh — a small educational shell written in C to teach process and job-control concepts.
- **Language:** C
- **License:** MIT
- **Build:** Make / gcc

This repository contains a pedagogical mini-shell (prompt `ebsh>`) demonstrating simple command parsing, execution of external programs, builtins (`cd`, `exit`) and basic job control (foreground/background) using signal handlers.

**Key features**
- Simple `ebsh>` prompt and read-eval loop.
- Builtins: `cd <dir>`, `exit`.
- External command execution with `fork()` + `execvp()`.
- Jobs: background execution with trailing `&` (one BG job supported in this version), start message and termination notification via `SIGCHLD`.
- `SIGINT` (Ctrl+C) handling forwarded to the foreground job.
- Minimal parser: tokenization by whitespace; no support for quotes, pipes, or redirections.

**Project structure**
```
├── main.c        # main loop of the shell
├── parse.c       # minimal parser + free_argv
├── parse.h
├── builtin.c     # cd, exit
├── builtin.h
├── jobs.c        # job control and signal handlers
├── jobs.h
├── Makefile      # (empty in repo) -> manual compilation possible
└── README_en.md
```

**Build**

The provided `Makefile` is empty; compile manually with `gcc`, or add a simple Makefile as shown below.

- Check you have a C compiler:
  - `gcc --version`

- Manual compilation (from project directory):
```bash
gcc -std=gnu11 -Wall -Wextra -pedantic -o ebsh main.c parse.c builtin.c jobs.c
```

- A minimal `Makefile` is now included in this repository. Use:
	- `make` — builds the `ebsh` executable.
	- `make clean` — removes the executable.



**Usage**

Run the shell interactively:
```bash
./ebsh
```

Examples inside the shell:

- External foreground command:
```
ebsh> pwd
```

- Builtin `cd`:
```
ebsh> cd ..
```

- Background job:
```
ebsh> sleep 5 &
Started background job with pid 12345
```

- Background job termination is printed by the SIGCHLD handler:
```
Background job exited with code 0
```

- Exit the shell:
```
ebsh> exit
```

Non-interactive example (scripted):
```bash
printf "pwd\ncd ..\npwd\nsleep 1 &\necho after\nexit\n" | ./ebsh
```

**Course context & learning objectives**

This project is part of an Operating Systems course and targets the following pedagogical objectives:

- Create processes using the `fork` function.
- Manage processes, notably avoiding zombie processes by reaping children (`waitpid`) and handling `SIGCHLD`.
- Handle signals sent to the shell (for example forwarding `SIGINT` to the foreground job, handling `SIGHUP`).

The mini-shell demonstrates these concepts through a simple parsing and execution loop, builtins (`cd`, `exit`), and basic job control (foreground/background).

**Tools & technologies**
- Language: C
- Compiler: gcc
- Dev OS: macOS (also works on Linux)
- VCS: Git & GitHub

**License**
MIT License — see `LICENSE` for the full text.


© 2025 Oriance Oternaud — University of Geneva (UNIGE)

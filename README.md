*This project has been created as part of the 42 curriculum by thanh-ng and gkhavari.*

# Minishell

## Description

**Minishell** (42 curriculum, subject version 10.0) is a small interactive Unix shell written in **C**. The goal is to understand **processes**, **file descriptors**, and how a real shell parses input, expands variables, and runs programs—without reimplementing full **bash**.

This repository implements the **mandatory** scope: prompt and **readline** history, **PATH** / relative / absolute execution, quotes (`'` and `"` with `$` still special inside `"`), redirections (`<`, `>`, `>>`, `<<`), **pipes**, environment variable expansion (`$VAR`, `$?`), signal handling (**Ctrl+C**, **Ctrl+D**, **Ctrl+\\** like bash in interactive mode), and builtins **`echo -n`**, **`cd`**, **`pwd`**, **`export`**, **`unset`**, **`env`**, **`exit`**. A single **global** stores only the last signal number (e.g. **`g_signum`**), per subject.

The subject does **not** require `\`, `;`, **`&&` / `||`**, or wildcard `*` in the mandatory part; this project follows that scope. **Bonus** (logical operators with parentheses and `*` wildcards) is only evaluated if the mandatory part is perfect—see subject **Chapter VI**.

For behavior details (exit codes, edge cases, tester alignment), see **[docs/BEHAVIOR.md](docs/BEHAVIOR.md)**. For architecture and data flow, see **[docs/MINISHELL_ARCHITECTURE.md](docs/MINISHELL_ARCHITECTURE.md)**.

---

## Instructions

### Dependencies

- **C compiler** (`cc` with `-Wall -Wextra -Werror` via the Makefile)
- **GNU readline** (and **ncurses** where the linker needs it)
  - **macOS (Homebrew):** `brew install readline` — if headers/libs are not found, point the Makefile at e.g. `/opt/homebrew/opt/readline` (see `Makefile` `RL_DIR` for Darwin).
  - **Linux:** packages such as `libreadline-dev` and `libncurses-dev` (names vary by distro).

### Compilation and installation

From the repository root:

```bash
make
```

Produces the **`minishell`** binary. Optional debug build (e.g. for Valgrind):

```bash
make debug
```

The Makefile builds **libft** first, then links **minishell**. Targets include at least **`all`**, **`clean`**, **`fclean`**, **`re`**, and **`$(NAME)`**, per 42 common instructions.

### Execution

```bash
./minishell
```

With a TTY you get a prompt and **readline** (history, line editing). Exit with **`exit`** or **Ctrl+D**. **Ctrl+C** starts a new line with a new prompt; **Ctrl+\\** is ignored at the prompt, bash-style.

**Examples:**

```bash
echo hello
echo -n "no newline"
pwd
cd /tmp
env
export MY_VAR=value
echo $MY_VAR
unset MY_VAR
echo hello | cat
echo hi > /tmp/out.txt
cat < /tmp/out.txt
cat << EOF
here doc
EOF
exit
```
---

## Resources

| Resource | Why it helps |
|----------|----------------|
| [GNU Bash manual](https://www.gnu.org/software/bash/manual/) | Reference for interactive behavior, builtins, expansion. |
| `man` pages: **`readline`**, **`signal`**, **`sigaction`**, **`pipe`**, **`dup2`**, **`fork`**, **`waitpid`**, **`execve`**, **`open`**, **`errno`** | Direct API and semantics for the allowed functions. |
| *Advanced Programming in the UNIX Environment* (Stevens) | Processes, FDs, signals—classic background for the subject. |
| [LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester) | Automated comparison against **bash** for mandatory/bonus modes. |
| In-repo **[docs/MINISHELL_ARCHITECTURE.md](docs/MINISHELL_ARCHITECTURE.md)**, **[docs/DATA_MODEL_AND_FUNCTIONS.md](docs/DATA_MODEL_AND_FUNCTIONS.md)**, **[docs/BEHAVIOR.md](docs/BEHAVIOR.md)** | Design, structs/functions catalog, and test-oriented behavior notes. |

**Use of AI:** AI-assisted tools may be used for **documentation** (e.g. README structure, wording), **explaining** APIs or subject text, and **suggesting** refactors or tests. **Implementation and correctness** are validated against the official subject, **Norm**, and the **42_minishell_tester** (and manual checks).

---

## Project structure (overview)

```
minishell/
├── src/
│   ├── main.c              # REPL, read_input
│   ├── core/               # init_shell, prompt, process_input
│   ├── utils/, free/
│   ├── signals/
│   ├── tokenizer/          # tokens, quotes, expansion
│   ├── parser/             # commands, heredoc, syntax
│   ├── executor/           # builtins, external, pipeline
│   └── builtins/
├── includes/               # minishell.h, structs.h, prototypes.h
├── libft/
├── scripts/                # run_minishell_tester.sh, helpers
├── docs/                   # architecture, data model, behavior
├── Makefile
└── README.md
```

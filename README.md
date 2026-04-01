# minishell

A minimal Unix shell written in C (42 project). Reads commands from the user, parses them, expands variables, and runs builtins or external programs with support for pipes and redirections.

---

## Overview

- **Prompt** with current user and directory; input via `readline()` (history supported).
- **Tokenizer** splits input into words and operators (`|`, `<`, `>`, `>>`, `<<`); handles single and double quotes and variable expansion (`$VAR`, `$?`).
- **Parser** builds a list of commands (with arguments and redirections), checks syntax, and finalizes `argv` and builtin detection.
- **Heredocs** are read before execution; variables are expanded unless the delimiter is quoted.
- **Executor** runs a single command (builtin in parent, external in a child) or a pipeline (each command in a child); applies redirections and propagates exit codes (pipeline = last command’s status).
- **Builtins:** `echo` (with `-n`), `cd`, `pwd`, `env`, `export`, `unset`, `exit`.
- **Signals:** Ctrl+C → new prompt and exit status 130; Ctrl+D → exit; Ctrl+\ ignored at prompt.

The project does **not** implement `&&`, `||`, or `;` as command separators (out of 42 mandatory scope).

---

## Requirements

- **C compiler** (e.g. `cc` / `gcc`)
- **readline** (and ncurses where needed)
  - **macOS:** `brew install readline` (link with `-L/opt/homebrew/opt/readline/lib -I/opt/homebrew/opt/readline/include` if needed)
  - **Linux:** `libreadline-dev` (and `libncurses-dev` if required)

---

## Build

From the repository root:

```bash
make
```

Produces the `minishell` executable. For debug build (e.g. for Valgrind):

```bash
make debug
```

> ⚠️ If you see a `_codeql_detected_source_root` entry in the repo root, it is a CodeQL artifact (a symlink to `.`). It can safely be removed (and is already ignored by `.gitignore`).
> 
> ```bash
git rm --cached --force _codeql_detected_source_root && rm -f _codeql_detected_source_root
```

---

## Usage

Run the shell:

```bash
./minishell
```

You’ll get a prompt (e.g. `user@minishell:/path$ `). Type commands as in bash (no `&&`/`||`/`;`). Exit with `exit` or Ctrl+D.

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

## Testing

Primary harness: **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** — same as **GitHub Actions** (`.github/workflows/test.yaml`).

### Run tests (recommended)

From the repo root, with the **Docker dev container** running (`docker compose up -d`):

```bash
./scripts/run_minishell_tester.sh m       # mandatory (default)
./scripts/run_minishell_tester.sh vm      # mandatory + valgrind
./scripts/run_minishell_tester.sh b         # bonus
./scripts/run_minishell_tester.sh ne        # empty-env
./scripts/run_minishell_tester.sh a         # all
```

The script builds inside the container (`make re` or `make debug` for valgrind modes) and runs `tester.sh --no-update <mode>`. The tester is cloned once into the container at `/root/42_minishell_tester` if missing.

To use another fork, edit the `git clone` URL in `scripts/run_minishell_tester.sh` (or maintain a local clone and mount it). Optional: `./scripts/push_tester_fork.sh` if you track a personal tester fork.

### Optional `tests/` Makefile

If your tree includes a `tests/` directory with a Makefile (some clones add it), you can still run:

```bash
make -C tests test        # wrapper, if present
make -C tests help
```

That path is **not** required when using only `run_minishell_tester.sh`.

### Input mode

When stdin is a TTY the shell uses **readline(prompt)**; when not (e.g. the tester) it reads stdin line-by-line in shell code (no readline dependency in non-interactive mode). Heredoc uses non-readline reads when `!isatty(stdin)`, and unclosed quotes produce an immediate syntax error (no multiline continuation).  

Scripts under `scripts/` require **git** and work on **macOS and Linux**. If git is missing, set `AUTO_INSTALL_DEPS=1` to try installing it.

---

## Project structure

```
minishell/
├── src/
│   ├── main.c              # REPL loop, read_input (only file in src/ root)
│   ├── core/
│   │   ├── init.c          # init_shell, process_input
│   │   ├── init_runtime.c
│   │   └── init_utils.c    # get_env_value, build_prompt
│   ├── utils/
│   │   ├── utils.c         # ft_arrdup, msh_calloc, ft_strcat, ft_realloc
│   │   └── utils2.c        # clean_exit, msh_strdup
│   ├── free/
│   │   ├── free_utils.c    # free_tokens, free_args
│   │   ├── free_runtime.c  # free_commands
│   │   └── free_shell.c    # reset_shell, free_all
│   ├── signals/
│   │   ├── signal_handler.c
│   │   └── signal_utils.c
│   ├── tokenizer/
│   │   ├── tokenizer.c
│   │   ├── tokenizer_utils.c, tokenizer_utils2.c
│   │   ├── tokenizer_ops.c, tokenizer_handlers.c, tokenizer_quotes.c
│   │   └── expansion.c, expansion_utils.c
│   ├── parser/
│   │   ├── parser.c
│   │   ├── parser_syntax_check.c
│   │   ├── add_token_to_cmd.c
│   │   ├── argv_build.c
│   │   ├── heredoc.c, heredoc_utils.c, heredoc_warning.c
│   │   └── ...
│   ├── executor/
│   │   ├── executor.c, executor_utils.c, executor_external.c
│   │   ├── executor_pipeline.c, executor_pipeline_steps.c
│   │   ├── executor_pipeline_not_found.c
│   │   ├── executor_child_exec.c, executor_child_format.c
│   │   └── ...
│   └── builtins/
│       ├── builtin_dispatcher.c
│       ├── echo.c, cd.c, pwd.c, env.c
│       ├── export.c, export_utils.c, export_print.c
│       ├── unset.c, exit.c, exit_utils.c
├── includes/
│   ├── minishell.h
│   ├── structs.h
│   └── prototypes.h
├── libft/                   # Static library (submodule or vendored)
├── tests/                   # optional: Makefile wrapper for tester, if present
├── scripts/
│   ├── run_minishell_tester.sh  # LeaYeh tester in Docker (m, vm, …)
│   ├── push_tester_fork.sh
│   └── ensure_deps.sh
├── docs/
│   ├── MINISHELL_ARCHITECTURE.md # Design, flow, Mermaid diagrams
│   ├── DATA_MODEL_AND_FUNCTIONS.md
│   ├── TECHNICAL_DECISIONS.md
│   ├── BEHAVIOR.md
│   └── 42_tester_failures.md
├── Makefile
└── README.md
```

---

## Documentation

| Doc | Description |
|-----|-------------|
| [docs/MINISHELL_ARCHITECTURE.md](docs/MINISHELL_ARCHITECTURE.md) | Architecture, source layout, main loop, parser, executor; Mermaid diagrams; implementation status. |
| [docs/DATA_MODEL_AND_FUNCTIONS.md](docs/DATA_MODEL_AND_FUNCTIONS.md) | Data model (why each struct/enum); function reference (every function by file). |
| [docs/TECHNICAL_DECISIONS.md](docs/TECHNICAL_DECISIONS.md) | What we changed and why: data, functions, defensive/bug prevention, 42 constraints. |
| [docs/BEHAVIOR.md](docs/BEHAVIOR.md) | Redirections, pipes, expansion, builtins, exit codes, signals. |

---

## Constraints (42 subject)

- **One global variable** only (e.g. `g_signum` for the signal number).
- **No memory leaks** (except known readline reachable blocks).
- **Stability:** no crashes (segfault, bus error, double free).
- **Norm:** code must follow the 42 Norm where required.

---

## License

This project is part of the 42 curriculum. Check your campus rules for reuse and attribution.

## Dev Container (Docker)

- This repository now contains a ready-to-use VS Code Dev Container based on the 42 Docker DevEnv. The `.devcontainer` directory was added from https://github.com/LeaYeh/42-Docker-DevEnv and includes a `Dockerfile` and `devcontainer.json` that install common 42 tooling (compilers, valgrind, readline, norminette, etc.).
- Prerequisites: Docker Desktop and the VS Code extensions **Remote - Containers** and **Dev Containers**.
- To open the project in the container from VS Code: open the Command Palette and choose `Dev Containers: Reopen in Container` (or `Remote-Containers: Reopen in Container`).
- If you prefer to (re)download the upstream devcontainer files, run:

```bash
bash -c "$(curl -fsSL https://raw.githubusercontent.com/LeaYeh/42-Docker-DevEnv/main/install.sh)"
```

- To build the container manually (first time) you can run the helper from the upstream repo:

```bash
bash -c "$(curl -fsSL https://raw.githubusercontent.com/LeaYeh/42-Docker-DevEnv/main/run.sh)"
```

- Once the container is running, your workspace will be mounted into the container at `/app` and you can build and run the project as on Linux (for example, `make`, `make debug`, `./minishell`, and the tester scripts).

If you want a lighter-weight devcontainer instead of the full 42 image, tell me and I can create a minimal `Dockerfile` that only installs the packages needed by this project.

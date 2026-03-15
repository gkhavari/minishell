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

Automated tests live under `tests/`. Run them from the **repository root** (so `./minishell` exists).

### Run all tests (recommended)

```bash
make -C tests test
```

This runs the **LucasKuhn/minishell_tester** suite (builtins, pipes, redirects, extras) plus the project’s consolidated **local_tests** (from former phase1, hardening, behavior). The runner clones the tester into `tests/lucas_minishell_tester` if needed and injects `tests/local_tests` as the `local` file. All tests compare minishell output and exit code to bash.

### Other targets

```bash
make -C tests test_lucas   # Full suite (Lucas + local_tests)
make -C tests test_local   # Only tests/local_tests
make -C tests help         # List targets
make -C tests clean        # Remove test binaries if present
```

### LucasKuhn/minishell_tester (primary suite)

[LucasKuhn/minishell_tester](https://github.com/LucasKuhn/minishell_tester) is the project’s main test suite. It lives under `tests/`: the runner clones it into `tests/lucas_minishell_tester` (gitignored) and adds `tests/local_tests` so one run covers both Lucas’s tests and the project’s own cases. Run from repo root:

```bash
make -C tests test
# or
./tests/run_lucas_tester.sh
```

To run only Lucas’s builtins/pipes/redirects/extras (no local file), use a fresh clone or run `./tester builtins` etc. from inside `tests/lucas_minishell_tester`. Override clone URL with `LUCAS_MINISHELL_TESTER_REPO=<url>` if you use a fork.

### 42_minishell_tester (optional)

CI and local runs use the [cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester) of [42_minishell_tester](https://github.com/zstenger93/42_minishell_tester) (RUNDIR + MINISHELL_PATH fixed for in-repo use). The fork can be updated with subject-aligned tests. From the project root, if `minishell_tester/` is missing, the script clones it for you.

```bash
./scripts/run_minishell_tester.sh m       # mandatory tests
./scripts/run_minishell_tester.sh vm      # mandatory + valgrind
./scripts/run_minishell_tester.sh m b     # mandatory, builtins only
```

To use the modified fork (RUNDIR + MINISHELL_PATH fixed for in-repo use):

```bash
export COZYGARAGE_TESTER_REPO="https://github.com/cozyGarage/42_minishell_tester.git"
./scripts/run_minishell_tester.sh m
```

To push further changes to the fork: `./scripts/push_tester_fork.sh`

Scripts under `scripts/` (and `tests/run_lucas_tester.sh`) require **git**. They work on both **macOS and Linux**. If git is missing, set `AUTO_INSTALL_DEPS=1` to try installing it (Homebrew on macOS, apt-get on Linux). For the Lucas tester timeout on macOS, install coreutils: `brew install coreutils` (provides `gtimeout`).

**Input mode:** When stdin is a TTY the shell uses **readline(prompt)**; when not (e.g. the tester) it uses **get_next_line** (in `libft/`, included via `minishell.h`) so line-by-line input matches the tester. Continuation and heredoc use non-readline reads when `!isatty(stdin)` (e.g. `fgets` in continuation, `read()` loop in heredoc).

---

## Project structure

```
minishell/
├── src/
│   ├── main.c              # REPL loop, read_input, process_input
│   ├── init.c              # init_shell, build_prompt, get_env_value
│   ├── utils.c             # ft_arrdup, msh_calloc, etc.
│   ├── free_utils.c         # free_tokens, free_args
│   ├── free_runtime.c      # free_commands, redirs
│   ├── free_shell.c        # reset_shell, free_all
│   ├── signals/
│   │   ├── signal_handler.c
│   │   └── signal_utils.c
│   ├── tokenizer/
│   │   ├── tokenizer.c
│   │   ├── tokenizer_utils.c, tokenizer_utils2.c
│   │   ├── tokenizer_ops.c, tokenizer_handlers.c, tokenizer_quotes.c
│   │   ├── expansion.c, expansion_utils.c
│   │   └── continuation.c
│   ├── parser/
│   │   ├── parser.c
│   │   ├── parser_syntax_check.c
│   │   ├── add_token_to_cmd.c
│   │   ├── argv_build.c
│   │   ├── heredoc.c
│   │   └── heredoc_utils.c
│   ├── executor/
│   │   ├── executor.c
│   │   ├── executor_utils.c
│   │   ├── executor_external.c
│   │   ├── executor_pipeline.c
│   │   └── executor_child.c
│   └── builtins/
│       ├── builtin_dispatcher.c
│       ├── echo.c, cd.c, pwd.c, env.c
│       ├── export.c, export_utils.c, export_print.c
│       ├── unset.c, exit.c
├── includes/
│   ├── minishell.h
│   ├── structs.h
│   └── prototypes.h
├── libft/                   # Static library (submodule or vendored)
├── tests/
│   ├── Makefile             # make test (= Lucas + local_tests), test_lucas, test_local
│   ├── local_tests           # Consolidated project tests (Lucas format; injected as "local")
│   └── run_lucas_tester.sh   # LucasKuhn/minishell_tester runner (injects local_tests)
├── scripts/
│   ├── run_minishell_tester.sh  # 42_minishell_tester runner (mandatory, valgrind)
│   ├── push_tester_fork.sh      # Push tester changes to fork
│   └── ensure_deps.sh           # Git check; optional install (AUTO_INSTALL_DEPS=1)
├── docs/
│   ├── minishell_architecture.md   # Design, flow, Mermaid diagrams
│   ├── DATA_MODEL_AND_FUNCTIONS.md # Struct/enum rationale + function reference
│   ├── TECHNICAL_DECISIONS.md      # What we changed and why (data, functions, defensive, 42)
│   └── BEHAVIOR.md                 # Edge cases and bash-aligned behavior
├── Makefile
└── README.md
```

---

## Documentation

| Doc | Description |
|-----|-------------|
| [docs/minishell_architecture.md](docs/minishell_architecture.md) | Architecture, source layout, main loop, parser, executor; Mermaid diagrams; implementation status. |
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

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

Automated tests live under `tests/`. Run them from the **repository root** (so `./minishell` exists).

### Run tests (recommended)

The project relies on **[42_minishell_tester](https://github.com/zstenger93/42_minishell_tester)** (cozyGarage fork) as the primary test suite. From the project root:

```bash
make -C tests test
```

This runs the **mandatory** tests from the 42_minishell_tester. If `minishell_tester/` is missing, the script clones the [cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester) for you.

```bash
./scripts/run_minishell_tester.sh m       # mandatory (same as make -C tests test)
./scripts/run_minishell_tester.sh vm      # mandatory + valgrind
./scripts/run_minishell_tester.sh m b     # mandatory, builtins only
```

To use a different fork: `export COZYGARAGE_TESTER_REPO="https://github.com/.../42_minishell_tester.git"` then run the script. To push changes to the fork: `./scripts/push_tester_fork.sh`.

### Other targets

```bash
make -C tests test_42     # Same as test
make -C tests help        # List targets
make -C tests clean      # Remove test binaries if present
```

### Input mode

When stdin is a TTY the shell uses **readline(prompt)**; when not (e.g. the tester) it uses **get_next_line** (in `libft/`, included via `minishell.h`) so line-by-line input matches the tester. Continuation and heredoc use non-readline reads when `!isatty(stdin)` (e.g. `fgets` in continuation, `read()` loop in heredoc).  

Scripts under `scripts/` require **git** and work on **macOS and Linux**. If git is missing, set `AUTO_INSTALL_DEPS=1` to try installing it.

---

## Project structure

```
minishell/
├── src/
│   ├── main.c              # REPL loop, read_input, process_input (only file in src/ root)
│   ├── core/
│   │   └── init.c          # init_shell, build_prompt, get_env_value
│   ├── utils/
│   │   └── utils.c         # ft_arrdup, msh_calloc, ft_strcat, ft_realloc
│   ├── free/
│   │   ├── free_utils.c    # free_tokens, free_args
│   │   ├── free_runtime.c  # free_commands, redirs
│   │   └── free_shell.c    # reset_shell, free_all
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
│   └── Makefile             # make test (= 42 mandatory), test_42
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

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

This runs:

1. **Phase 1** (`tests/test_phase1.sh`) — 24 tests: foundation and builtins (echo, pwd, cd, env, export, unset, exit).
2. **Hardening** (`tests/test_hardening.sh`) — 106 tests: empty input, syntax errors, expansion, redirections, pipes, heredocs, exit codes, path resolution, edge cases.

Both suites must pass (no failures).

### Run suites separately

```bash
make -C tests test_phase1    # Phase 1 only
make -C tests test_hardening # Hardening only
```

### Other targets

```bash
make -C tests help   # List test targets
make -C tests clean  # Remove test binaries (e.g. test_builtins if present)
```

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
│   ├── Makefile             # make test, test_phase1, test_hardening
│   ├── test_phase1.sh
│   └── test_hardening.sh
├── docs/
│   ├── minishell_architecture.md   # Design, data structures, Mermaid diagrams
│   ├── BEHAVIOR.md                 # Edge cases and bash-aligned behavior
│   └── MANDATORY_TEST_FAILURES.md  # 42 tester / Valgrind failure analysis
├── Makefile
└── README.md
```

---

## Documentation

| Doc | Description |
|-----|-------------|
| [docs/minishell_architecture.md](docs/minishell_architecture.md) | Architecture, source layout, main loop, parser, executor; Mermaid diagrams; implementation status. |
| [docs/BEHAVIOR.md](docs/BEHAVIOR.md) | Redirections, pipes, expansion, builtins, exit codes, signals. |
| [docs/MANDATORY_TEST_FAILURES.md](docs/MANDATORY_TEST_FAILURES.md) | Why 42 mandatory/Valgrind tests fail (e.g. `&&`/`||` out of scope vs real bugs). |

---

## Constraints (42 subject)

- **One global variable** only (e.g. `g_signum` for the signal number).
- **No memory leaks** (except known readline reachable blocks).
- **Stability:** no crashes (segfault, bus error, double free).
- **Norm:** code must follow the 42 Norm where required.

---

## License

This project is part of the 42 curriculum. Check your campus rules for reuse and attribution.

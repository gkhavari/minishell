*This project has been created as part of the 42 curriculum by thanh-ng, gkhavari.*

# minishell

A minimal Unix shell written in C (42 project). The implementation follows the 42 subject: it tokenizes and parses input, performs variable expansion, supports pipes and redirections, handles heredocs, and runs builtins or external programs.

## Description

- Prompt with current user and working directory (interactive mode).
- Tokenizer handles words and operators (`|`, `<`, `>`, `>>`, `<<`), single and double quotes, and variable expansion (`$VAR`, `$?`).
- Parser builds command structures with arguments and redirections and validates syntax.
- Heredocs are supported and processed before execution; quoted delimiters affect expansion as required by the subject.
- Executor runs builtins when appropriate and external programs in child processes; pipelines use pipes between commands.
- Implemented builtins: `echo` (with `-n`), `cd`, `pwd`, `env`, `export`, `unset`, `exit`.
- Signals: Ctrl+C displays a fresh prompt and sets status 130; Ctrl+D exits; Ctrl+\\ ignored at prompt.

## Requirements

- C compiler (`cc`/`gcc`) and standard build tools.
- `readline` development headers/libraries (`libreadline-dev` on Linux).

### Ubuntu

On Ubuntu/Debian install the build and readline dev packages:

```bash
sudo apt update && sudo apt install build-essential libreadline-dev
```

## Build

From the repository root:

```bash
make
```

For a debug build (useful with Valgrind):

```bash
make debug
```

This produces the `minishell` executable in the repository root.

### Style (Norminette)

This project follows the 42 coding style. You can run the Norminette style checker with:

```bash
make norm
# or
norminette src includes libft
```

The `Makefile` includes a `norm` target that invokes `norminette` on `src`, `includes`, and `libft` (if present). Install `norminette` via your package manager or the official installer if it's not available.

## Usage

Run the shell:

```bash
./minishell
```

The shell provides a prompt, history (interactive mode), redirections, pipelines, heredocs, and variable expansion per the subject.

## Project Structure (high level)

- `src/` — source files (main loop, tokenizer, parser, executor, builtins, utils)
- `includes/` — headers and prototypes
- `libft/` — vendored libft utilities
- `tests/` — optional tests (not required for submission)

See `docs/` for detailed architecture, data model, and technical decisions.

## Constraints & Notes (42 subject)

- Only one global variable is used for signals.
- Avoid crashes (segfaults, double frees) and eliminate heap leaks in your code (readline's reachable allocations are accepted by the subject).
- The mandatory scope does not require `&&`, `||`, or `;` command separators.

## Authors

- thanh-ng
- gkhavari

## Resources

- 42 subject `minishell` (project specification)
- `readline` documentation
- Design and decisions: `docs/`

AI assistance: AI was used to help draft documentation and assist with small refactors; the authors implemented and validated all code changes.

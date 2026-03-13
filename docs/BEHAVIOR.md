# Minishell Behavior & Edge Cases

This document explicitly outlines the intended behavior of the minishell, specifically how it handles edge cases, to ensure that actions are deliberate and justifiable during evaluations.

## 1. Redirections (`<`, `>`, `>>`, `<<`)

- **Evaluation Order**: Redirections are evaluated left-to-right before the core command is executed.
- **Multiple Output Redirections (`> a > b`)**: All files are opened (and created/truncated) in the order they appear. The command's standard output is ultimately directed to the _last_ successful output redirection in the chain. For example, `echo hi > a > b` will create both `a` and `b`, but only `b` will contain "hi".
- **Missing or Invalid Input Files**: In minishell, input redirections are verified and validated prior to executing output file creations as an early-fail safety net. If an input file is missing (e.g., `< missing`), the command immediately fails, and no output files will be created or truncated, regardless of their left-to-right positional syntax (e.g., `> out1 < missing > out2` will not create `out1`). The sequence is exactly:
  1.  When `< missing` is encountered, `open()` fails.
  2.  An error message is printed to `stderr` (e.g., `minishell: No such file or directory`).
  3.  Redirection processing halts immediately. Remaining redirections are _not_ executed, and the base command (`cat`) is _not_ executed.
  4.  The exit status is set to `1`.
- **Heredocs (`<<`)**: Heredocs are fully read and processed during the parsing phase, _before_ execution begins.
  - If multiple heredocs exist for a single command, all are read from the user, but only the contents of the _last_ one will be fed as `stdin` to the command.
  - Variables are expanded inside heredocs unless the delimiter is quoted (e.g., `<< 'EOF'`).

## 2. Pipes (`|`)

- **Execution Model**: All commands in a pipeline are executed as child processes. This means builtins executed in pipelines (e.g., `cd /tmp | cat`) do not affect the main shell environment.
- **Exit Status**: The exit code of a pipeline is the exit code of the _last_ command in the pipeline (e.g., `false | true` results in exit `0`; `true | false` results in exit `1`).
- **Empty Pipeline Sides**: Syntax like `| cat` or `cat |` is considered an invalid grammatical construct, resulting in a syntax error and an exit code of `2`.

## 3. Variable Expansion (`$`)

- **Undefined Variables**: Expanding an undefined variable results in an empty string (e.g., `echo $UNDEFINED` prints an empty line and does not crash).
- **In Quotes**:
  - Double quotes (`"..."`): Variables are expanded.
  - Single quotes (`'...'`): Variables are literally interpreted (no expansion occurs).
- **`$?`**: Expands to the exit code of the last executed command or pipeline.

## 4. Builtins

- **`exit`**:
  - `exit <numeric>`: Exits with `numeric % 256`. It properly parses signs and handles trailing/leading whitespace. It employs modular arithmetic to entirely avoid internal C-level overflow limitations.
  - `exit <non-numeric>`: Prints `numeric argument required` to `stderr`, and forces the shell to exit with code `255`.
  - `exit <numeric> <anything>`: Prints `too many arguments` to `stderr`, does _not_ close the shell, and sets the exit code to `1`.
- **`cd`**:
  - Automatically updates `PWD` and `OLDPWD` in the environment to intuitively reflect directory changes.
  - Generates a descriptive error and returns `1` if the target directory doesn't exist or lacks permissions.
- **`export`**:
  - Without arguments, prints environment variables sorted alphabetically, prefixed with `declare -x ` to precisely match bash behavior.
  - Rejects invalid identifiers (must begin with a letter or underscore, and contain only alphanumerics and underscores) and prints `not a valid identifier`.
- **`echo`**:
  - Correctly supports the `-n` flag, including repeated flags (e.g., `-nnnn`), and halts flag processing as soon as a non-flag argument is encountered.

## 5. Signals

- **`Ctrl-C` (`SIGINT`)**:
  - _Interactive prompt_: Prints a newline, displays a fresh prompt, clears the current input buffer, and sets `$?` to `130`.
  - _During execution_: Safely interrupts the running child processes and sets `$?` to `130`.
- **`Ctrl-D` (EOF on stdin)**: Gracefully exits the shell, returning the status of the last executed command.
- **`Ctrl-\` (`SIGQUIT`)**:
  - _Interactive prompt_: Does entirely nothing (is ignored).
  - _During execution_: Terminates the running child processes, generates a "Quit" message (or core dump message depending on system settings), and sets `$?` to `131`.

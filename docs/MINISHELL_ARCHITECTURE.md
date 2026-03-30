# Minishell Architecture

> A study guide for the minishell codebase. Covers pipeline stages, source layout, signal handling, and how each module works together.
>
> **Companion docs:** [DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md) (structs + function reference), [BEHAVIOR.md](BEHAVIOR.md) (expected I/O and exit codes).

---

## 1. Source Layout

| Directory | Files | Purpose |
|-----------|-------|---------|
| `src/` | `main.c` | REPL loop, `read_input`, `process_input` |
| `src/core/` | `init.c`, `init_runtime.c` | Shell initialization, env setup, SHLVL, OLDPWD |
| `src/utils/` | `utils.c` | `ft_strcat`, `ft_realloc`, `ft_arrdup`, `msh_calloc` |
| `src/free/` | `free_utils.c`, `free_runtime.c`, `free_shell.c` | Memory cleanup (tokens, commands, shell) |
| `src/signals/` | `signal_handler.c`, `signal_utils.c` | Signal setup, readline hook, child exit handling |
| `src/tokenizer/` | `tokenizer.c`, `tokenizer_utils.c`, `tokenizer_utils2.c`, `tokenizer_ops.c`, `tokenizer_handlers.c`, `tokenizer_quotes.c`, `expansion.c`, `expansion_utils.c`, `continuation.c` | Lexer: input string to token list |
| `src/parser/` | `parser.c`, `parser_syntax_check.c`, `add_token_to_cmd.c`, `argv_build.c`, `heredoc.c`, `heredoc_utils.c` | Parser: tokens to command list + heredocs |
| `src/executor/` | `executor.c`, `executor_utils.c`, `executor_cmd_utils.c`, `executor_external.c`, `executor_child.c`, `executor_child_exec.c`, `executor_child_format.c`, `executor_pipeline.c`, `executor_pipeline_steps.c`, `executor_count.c` | Executor: run commands, pipes, redirections |
| `src/builtins/` | `builtin_dispatcher.c`, `echo.c`, `cd.c`, `pwd.c`, `env.c`, `export.c`, `export_utils.c`, `export_print.c`, `unset.c`, `exit.c` | Builtin commands |
| `includes/` | `minishell.h`, `includes.h`, `defines.h`, `structs.h`, `prototypes.h` | Headers |
| `libft/` | 50+ files | 42-approved utility library |

**Total: 45 source files, 5 headers.**

---

## 2. High-Level Pipeline

```
main()
  |
  +-- init_shell(envp)         # Duplicate env, set USER/CWD/SHLVL/OLDPWD/had_path
  +-- set_signals_interactive() # SIGINT -> g_signum; ignore SIGQUIT/SIGTERM/SIGPIPE
  |
  +-- shell_loop()
  |     |
  |     +-- check_signal_received()   # If SIGINT: last_exit=130, reset g_signum
  |     +-- read_input()              # TTY: readline(); non-TTY: read_line_stdin()
  |     +-- process_input()
  |     |     |
  |     |     +-- tokenize_input()     # Input string -> token linked list
  |     |     +-- parse_input()        # Syntax check -> command linked list -> argv
  |     |     +-- process_heredocs()   # Read heredoc bodies into pipes
  |     |     +-- execute_commands()   # Single command or pipeline
  |     |
  |     +-- reset_shell()             # Free tokens, commands, input
  |     +-- (loop)
  |
  +-- free_all() + exit(last_exit)
```

---

## 3. Global State and Signals

### 3.1 The Only Global Variable

```c
volatile sig_atomic_t g_signum = 0;   /* signals/signal_handler.c */
```

42 subject allows **one** global. It stores the signal number (0 = none, SIGINT = 2). Checked and cleared in `check_signal_received()` after `readline()` returns.

### 3.2 Signal Modes

| Mode | When | SIGINT | SIGQUIT | SIGTERM | SIGPIPE |
|------|------|--------|---------|---------|---------|
| **Interactive** | At prompt | `g_signum = SIGINT`, newline | Ignored | Ignored | Ignored |
| **Ignore** | Parent during pipeline | Ignored | Ignored | -- | -- |
| **Default** | Child before execve | Default (kill) | Default (core) | Default | Default |

### 3.3 Signal Flow

```
Ctrl+C at prompt:
  -> interactive_sigint_handler() sets g_signum = SIGINT
  -> readline returns (via rl_event_hook)
  -> check_signal_received() sets last_exit = 130, clears g_signum
  -> New prompt displayed

Ctrl+C during execution:
  -> Child killed by SIGINT
  -> Parent receives SIGCHLD, waitpid gets status
  -> handle_child_exit() computes 128 + SIGINT = 130

Ctrl+D (EOF):
  -> readline returns NULL
  -> Print "exit\n" (TTY only), break from loop
```

---

## 4. Tokenizer (Lexer)

**Entry:** `tokenize_input()` in `tokenizer/tokenizer.c`

Scans `shell->input` character-by-character, producing a linked list of `t_token` nodes.

### 4.1 Token Types

| Type | Symbol | Example |
|------|--------|---------|
| `WORD` | (text) | `echo`, `hello`, `file.txt` |
| `PIPE` | `\|` | `ls \| cat` |
| `REDIR_IN` | `<` | `< input.txt` |
| `REDIR_OUT` | `>` | `> output.txt` |
| `APPEND` | `>>` | `>> log.txt` |
| `HEREDOC` | `<<` | `<< EOF` |
# Note: numeric fd redirections like `2>` are not supported by this minishell.
# They are tokenized as a `WORD` containing the digit (e.g. `2`) followed by a
# `REDIR_OUT` token (`>`), so `2>file` is parsed as `WORD("2")` + `REDIR_OUT`.

### 4.2 State Machine

```
ST_NORMAL:
  ' -> enter ST_SQUOTE (no expansion, literal until closing ')
  " -> enter ST_DQUOTE (expand $VAR/$?, literal otherwise until ")
  | < > -> emit operator token
  Numeric fd forms like `2>` are not recognized as a single operator; they are
  tokenized as a `WORD` (the digit) followed by the `>` operator, so `2>file`
  becomes `WORD("2")` then `REDIR_OUT` (`>`).
  whitespace -> flush current word
  $ -> expand variable
  ~ -> expand to HOME (at word start)
  other -> append to current word

ST_SQUOTE:
  ' -> return to ST_NORMAL
  other -> append literal

ST_DQUOTE:
  " -> return to ST_NORMAL
  $ -> expand variable
  other -> append literal
```

### 4.3 Variable Expansion

Happens during tokenization in `expansion.c`:

| Input | Context | Result |
|-------|---------|--------|
| `$HOME` | Unquoted / double quotes | `/home/user` |
| `'$HOME'` | Single quotes | `$HOME` (literal) |
| `$?` | Any (except single quotes) | Last exit status |
| `$UNDEFINED` | Any | Empty string |
| `$` | End of word | `$` (literal) |
| `~` | Word start, unquoted | Value of `HOME` |

### 4.4 Continuation

If input has unclosed quotes, `continuation.c` reads more lines (using `readline("> ")` for TTY or `read()` for non-TTY) and appends to `shell->input`.

---

## 5. Parser

**Entry:** `parse_input()` in `parser/parser.c`

### 5.1 Syntax Validation

`syntax_check()` in `parser_syntax_check.c` validates:
- No leading/trailing/consecutive pipes
- Every redirection followed by a WORD token
- Prints `minishell: syntax error near unexpected token 'X'`, sets `last_exit = 2`

### 5.2 Parsing Flow

```
Tokens: [WORD:"echo"] [WORD:"hello"] [PIPE] [WORD:"cat"] [REDIR_OUT] [WORD:"out.txt"]

parse_tokens():
  cmd1: args=[echo, hello]  redirs=[]
  cmd2: args=[cat]          redirs=[{file="out.txt", fd=STDOUT, append=0}]

finalize_all_commands():
  cmd1: argv=["echo","hello",NULL]  is_builtin=1
  cmd2: argv=["cat",NULL]           is_builtin=0
```

- `add_token_to_command()` routes WORD tokens to `args` list, redirections to `redirs` list, heredoc delimiters to `heredoc_delim`
- `finalize_argv()` converts `args` linked list to `char **argv`
- `is_builtin()` checks `argv[0]` against builtin names

### 5.3 Heredoc Processing

Happens after parsing, before execution. `process_heredocs()` in `heredoc.c`:

1. For each command with `heredoc_delim` set:
2. Create a pipe
3. Read lines until delimiter (via `readline("> ")` or stdin)
4. If delimiter was unquoted, expand `$VAR` in body
5. Write body to pipe write end
6. Store pipe read end in `cmd->heredoc_fd`
7. SIGINT during heredoc: stop, return to prompt, `last_exit = 130`

---

Note on multiple heredocs:

If a single command contains multiple `<<` operators (for example
`cat << EOF1 << EOF2`), this minishell stores only one `heredoc_delim` per
command and will process only the last delimiter. Earlier `<<` delimiters are
overwritten while parsing. By contrast, bash reads all here-document bodies
for the command before execution; the minishell's behavior is an intentional
simplification and therefore a divergence from bash.


## 6. Executor

**Entry:** `execute_commands()` in `executor/executor.c`

### 6.1 Decision Tree

```
execute_commands()
  |
  +-- commands == NULL? -> return 0
  +-- commands->next == NULL? -> execute_single_command()
  +-- else -> execute_pipeline()
```

### 6.2 Single Command

```
execute_single_command(cmd, shell):
  1. backup_fds() -> save stdin/stdout
  2. apply_redirections(cmd) -> open files, dup2
  3. If no argv: restore FDs, return 0
  4. If builtin: run_builtin() in parent process
  5. If external: fork() + execute_in_child()
  6. restore_fds() -> restore stdin/stdout
  7. Return exit status
```

**Why builtins run in parent:** `cd` must change the parent's working directory, `export`/`unset` must modify the parent's environment, `exit` must terminate the parent process.

### 6.3 Pipeline

```
ls -la | grep ".c" | wc -l

execute_pipeline():
  1. Count commands (3)
  2. Setup barrier pipe (sync_fd) for synchronized launch
  3. For each command: run_pipe_step() -> fork child
     - Child: setup stdin/stdout from pipes, apply_redirections, execute_in_child
  4. Release barrier (all children start simultaneously)
  5. Close all pipe FDs in parent
  6. wait_pipeline() -> return exit status of LAST child
```

**Pipeline barrier:** `barrier_write_fd` in `t_shell`. Children block on reading from the barrier pipe until the parent closes it, so all children start at roughly the same time. This reduces non-deterministic stderr ordering.

### 6.4 Child Process Execution

`execute_in_child()` in `executor_child_exec.c`:

```
execute_in_child(cmd, shell):
  1. If builtin: run_builtin() then exit_child()
  2. find_command_path() -> search PATH or use absolute path
  3. If not found: print "command not found", exit(127)
  4. If directory: print "Is a directory", exit(126)
  5. set_underscore() -> update _ env var
  6. execve(path, argv, envp)
  7. If execve fails: exit(126) or exit(127)
```

### 6.5 Redirections

`apply_redirections()` in `executor_utils.c`:

1. Walk `cmd->redirs` list left to right
2. For each: open file, `dup2` to target fd (stdin, stdout, or stderr)
3. Handle ambiguous redirects (empty expansion -> error)
4. After all file redirections: if `heredoc_fd != -1` and no file input redirect was applied, `dup2(heredoc_fd, STDIN_FILENO)`
5. Close `heredoc_fd`

### 6.6 PATH Resolution

`find_command_path()` in `executor_external.c`:

```
find_command_path(cmd, shell):
  1. If cmd contains '/': check access(cmd, X_OK), return strdup or NULL
  2. Get PATH from env
  3. If no PATH: return NULL (-> 127)
  4. Split PATH by ':'
  5. For each dir: try dir/cmd, check is_regular_file + access(X_OK)
  6. Return first match or NULL
```

---

## 7. Builtin Commands

All builtins dispatch through `run_builtin()` in `builtin_dispatcher.c`.

| Builtin | File | Key Behavior |
|---------|------|-------------|
| `echo` | `echo.c` | Print args with spaces; `-n` suppresses newline; `-nnn` accepted |
| `cd` | `cd.c` | Change dir (arg, HOME, or `-` for OLDPWD); updates PWD/OLDPWD/cwd |
| `pwd` | `pwd.c` | Print `getcwd()` |
| `export` | `export.c` | No args: print sorted env (`export KEY="value"` format); with args: set env vars; validates names |
| `unset` | `unset.c` | Remove vars from envp; validates names |
| `env` | `env.c` | Print envp (KEY=value lines); rejects arguments |
| `exit` | `exit.c` | Exit with status (mod 256); non-numeric -> exit 2; too many args -> error, don't exit |

---

## 8. Memory Management

### 8.1 Per-Line Cleanup

`reset_shell()` in `free/free_shell.c` runs after each line:
- `free(shell->input)` -> NULL
- `free_tokens(shell->tokens)` -> NULL
- `free_commands(shell->commands)` -> NULL

### 8.2 Full Teardown

`free_all()` runs at process exit:
- Frees tokens, commands, envp, user, cwd, input

### 8.3 Child Cleanup

`exit_child()` in `executor_child.c`: calls `free_all()`, closes standard FDs, then `exit(status)`. Every child path must go through this to avoid leaks.

---

## 9. Exit Codes

| Scenario | Code | Notes |
|----------|------|-------|
| Success | `0` | |
| General error | `1` | Builtin failure, redir error |
| Syntax error | `2` | Invalid pipe/redir, `exit` non-numeric |
| Permission denied | `126` | File not executable, path is directory |
| Command not found | `127` | Not in PATH |
| Signal N | `128 + N` | e.g. SIGINT = 130, SIGQUIT = 131 |
| `exit N` | `N % 256` | Wraps: `exit 256` -> 0 |
| `exit non-numeric` | `2` | Bash uses 255; known delta |

---

## 10. Error Message Format

```
minishell: CONTEXT: MESSAGE
```

Examples:
```
minishell: syntax error near unexpected token `|'
minishell: cd: /nonexistent: No such file or directory
minishell: export: `1bad': not a valid identifier
minishell: nosuchcmd: command not found
minishell: ./dir: Is a directory
```

---

## 11. Build

```bash
make          # Build minishell (builds libft first)
make clean    # Remove object files
make fclean   # Remove executable + libft.a
make re       # Clean rebuild
make debug    # Build with -g for valgrind/gdb
```

**Compiler flags:** `-Wall -Wextra -Werror`
**Linux readline:** `/usr/lib` (CI/Ubuntu)
**macOS readline:** `/opt/homebrew/opt/readline` (Homebrew)

---

## 12. Testing

```bash
# Clone the tester
git clone https://github.com/LeaYeh/42_minishell_tester.git

# Run mandatory tests
bash tester.sh m

# Run with valgrind (Linux only)
bash tester.sh vm
```

**Current status (main, Ubuntu):** 981/986 passed, 0 leaks, 0 crashes. Remaining failures are non-deterministic pipeline stderr ordering.

# Minishell Expected Behavior

> A study guide covering what output and exit code the shell must produce for given inputs. Use this alongside [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) (pipeline stages) and [DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md) (structs + function reference).

---

## How the Tester Works

The [42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester) sends the same input to both **minishell** and **bash --posix**, then compares:

- **stdout** (`diff -q`)
- **stderr** (`diff -q` after normalization)
- **exit code** (must match exactly)

The tester feeds input non-interactively via stdin, so the shell uses `read_line_stdin()` (not readline). Test scripts live under `cmds/mand/`.

**Current status (main, Ubuntu with valgrind):** 981/986 passed, 0 leaks, 0 crashes. The 5 remaining failures are non-deterministic pipeline stderr ordering.

---

## 1. Echo

**Source:** `src/builtins/echo.c` | **Tester:** `1_builtins_echo.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `echo hello` | `hello\n` | none | 0 |
| `echo hello world` | `hello world\n` | none | 0 |
| `echo -n hello` | `hello` (no newline) | none | 0 |
| `echo -nnn hello` | `hello` (no newline) | none | 0 |
| `echo -n -n hello` | `hello` (no newline) | none | 0 |
| `echo` | `\n` (newline only) | none | 0 |
| `echo ""` | `\n` (newline only) | none | 0 |
| `echo -n` | (nothing) | none | 0 |
| `echo -n -a hello` | `-a hello\n` | none | 0 |

**How it works:** `check_n_flag()` accepts `-n`, `-nn`, `-nnn`, etc. (must be all `n` after `-`). Once a non-flag arg is seen, all remaining args are printed separated by spaces. The `-n` flag suppresses the trailing newline.

---

## 2. PWD

**Source:** `src/builtins/pwd.c` | **Tester:** `1_builtins_pwd.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `pwd` | Current working directory + `\n` | none | 0 |
| `pwd extra args` | Same as `pwd` (extra args ignored) | none | 0 |

**How it works:** Calls `getcwd(NULL, 0)` and prints the result. Extra arguments are ignored (matches bash).

---

## 3. CD

**Source:** `src/builtins/cd.c` | **Tester:** `1_builtins_cd.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `cd /tmp` then `pwd` | `/tmp\n` | none | 0 |
| `cd` (no args) | (none) | none | 0 (changes to `$HOME`) |
| `cd --` | (none) | none | 0 (changes to `$HOME`) |
| `cd -` | prints new cwd | none | 0 (changes to `$OLDPWD`) |
| `cd nonexistent` | (none) | `minishell: cd: nonexistent: No such file or directory` | 1 |
| `cd a b` (too many args) | (none) | `minishell: cd: too many arguments` | 1 |
| `cd` with HOME unset | (none) | `minishell: cd: HOME not set` | 1 |
| `cd -` with OLDPWD unset | (none) | `minishell: cd: OLDPWD not set` | 1 |

**How it works:** `get_cd_target()` resolves the target: no args / `--` uses `$HOME`, `-` uses `$OLDPWD` (and prints new cwd). Too many args is rejected before target resolution. On success, `update_shell_cwd()` updates `PWD`, `OLDPWD`, and `shell->cwd`.

**Note:** At init, if `OLDPWD` is not in the environment, it is added as a bare export entry (no value). This matches bash's behavior of showing `export OLDPWD` (without `=`) in the `export` listing.

---

## 4. Export

**Source:** `src/builtins/export.c`, `export_utils.c`, `export_print.c` | **Tester:** `1_builtins_export.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `export` (no args) | Sorted env in `export KEY="value"` format | none | 0 |
| `export A=1` then `env` | `A=1` appears in env | none | 0 |
| `export A` (no value) | (none; `A` shown without `=` in `export` list) | none | 0 |
| `export A+=suffix` | Appends `suffix` to existing value of `A` | none | 0 |
| `export 1BAD=x` | (none) | `export: '1BAD=x': not a valid identifier` | 1 |
| `export -p` | (none) | `export: -p: invalid option` | 2 |

**How it works:**

- **No args:** `print_sorted_env()` duplicates `envp`, sorts by key using bubble sort, prints each entry as `export KEY="value"` or `export KEY` (bare). The `_` variable is skipped.
- **With args:** Each arg is parsed for `=` or `+=`. Valid identifiers start with letter/underscore and contain only alphanumerics/underscores. Invalid names get an error; options starting with `-` get "invalid option" (exit 2).
- **Format note:** The tester uses `bash --posix`, which outputs `export KEY="value"` (not `declare -x`). Our implementation matches this POSIX format.

---

## 5. Unset

**Source:** `src/builtins/unset.c` | **Tester:** `1_builtins_unset.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `unset VAR` then `env` | `VAR` no longer in env | none | 0 |
| `unset` (no args) | (none) | none | 0 |
| `unset NONEXISTENT` | (none) | none | 0 |

**How it works:** Finds and removes matching entries from `shell->envp`. No error for nonexistent names. Validates identifier names (same rules as export).

---

## 6. Env

**Source:** `src/builtins/env.c` | **Tester:** `1_builtins_env.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `env` | All `KEY=value` entries, one per line | none | 0 |
| `env anything` | (none) | `env: 'anything': No such file or directory` | 127 |

**How it works:** Iterates `shell->envp` and prints only entries containing `=` (bare exports are hidden). Any argument triggers an error with exit 127 (the builtin `env` accepts no args; `env` with args is treated as an error, matching the subject requirement).

---

## 7. Exit

**Source:** `src/builtins/exit.c`, `exit_utils.c` | **Tester:** `1_builtins_exit.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `exit` | (none) | `exit\n` (TTY only) | last exit status |
| `exit 0` | (none) | `exit\n` (TTY only) | 0 |
| `exit 42` | (none) | `exit\n` (TTY only) | 42 |
| `exit 256` | (none) | `exit\n` (TTY only) | 0 (256 % 256) |
| `exit -1` | (none) | `exit\n` (TTY only) | 255 (unsigned 8-bit) |
| `exit hello` (non-numeric) | (none) | `exit\n` + `numeric argument required` | **2** |
| `exit 1 2` (too many args) | (none) | `exit\n` + `too many arguments` | 1 (does NOT exit) |

**How it works:**

1. Prints `exit` to stderr if interactive (TTY).
2. No args: exits with `shell->last_exit`.
3. Non-numeric arg: `parse_exit_value()` (uses `strtoll`) fails, prints error, exits with **2**.
4. Too many args: prints error, returns **1** without exiting.
5. Valid numeric: exits with `(unsigned char)value` (i.e., `value % 256`).

**Known bash delta:** Bash exits with **255** for non-numeric arguments. This minishell exits with **2**. The tester will flag this as an EXIT_CODE mismatch on those specific cases.

---

## 8. Variable Expansion

**Source:** `src/tokenizer/expansion.c`, `expansion_utils.c` | **Tester:** `0_compare_parsing.sh`, `1_variables.sh`

| Input | stdout | Exit | Notes |
|-------|--------|------|-------|
| `echo $HOME` | Value of HOME | 0 | Unquoted expansion |
| `echo "$HOME"` | Value of HOME | 0 | Double-quote expansion |
| `echo '$HOME'` | Literal `$HOME` | 0 | Single quotes: no expansion |
| `echo $?` | Last exit status | 0 | Special variable |
| `echo $UNSET_VAR` | (empty line) | 0 | Undefined expands to empty |
| `echo $` | `$` | 0 | Bare `$` at end of word |
| `echo ~` | Value of HOME | 0 | Tilde expansion (word start) |
| `echo "~"` | `~` | 0 | Quoted tilde: literal |

**How it works:** Expansion happens during tokenization. The tokenizer state machine tracks `ST_NORMAL`, `ST_SQUOTE`, `ST_DQUOTE`. In normal and double-quote states, `$` triggers variable lookup. Single quotes preserve everything literally. `$?` expands to `shell->last_exit`. Undefined variables expand to empty string.

---

## 9. Redirections

**Source:** `src/executor/executor_utils.c`, `src/parser/add_token_to_cmd.c` | **Tester:** `1_redirs.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `echo hello > file` then `cat file` | `hello` | none | 0 |
| `echo a > file; echo b >> file` then `cat file` | `a\nb` | none | 0 |
| `cat < file` (file exists) | file contents | none | 0 |
| `cat < nonexistent` | (none) | `nonexistent: No such file or directory` | 1 |
| `echo a > f1 > f2` | (none) | none | 0 (f1 created empty, f2 gets `a`) |
| `echo err 2> file` | (none to stdout) | none | 0 (stderr content in `file`) |
| `echo hi > /dev/full` | (none) | error message | 1 |

**How it works:**

- Redirections are stored as a `t_redir` linked list during parsing.
- `apply_redirections()` iterates the list left to right, calling `open()` + `dup2()` for each.
- `>` opens with `O_WRONLY | O_CREAT | O_TRUNC`, `>>` with `O_APPEND`.
- `<` opens with `O_RDONLY` and dup2's to `STDIN_FILENO`.
- `2>` opens for write and dup2's to `STDERR_FILENO`.
- Multiple output redirects: all files are created/truncated, but only the last one receives output (last `dup2` wins).
- Ambiguous redirects (empty expansion like `$UNSET > $EMPTY`) are detected by a prefix marker and produce "ambiguous redirect" errors.

---

## 10. Heredocs

**Source:** `src/parser/heredoc.c`, `heredoc_utils.c` | **Tester:** `1_redirs.sh`, `10_parsing_hell.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `cat << EOF\nhello\nEOF` | `hello` | none | 0 |
| `cat << EOF\n$HOME\nEOF` | Value of HOME | none | 0 |
| `cat << 'EOF'\n$HOME\nEOF` | Literal `$HOME` | none | 0 |

**How it works:**

1. Parser stores the delimiter in `cmd->heredoc_delim` and whether it was quoted in `cmd->heredoc_quoted`.
2. `process_heredocs()` runs after parsing, before execution.
3. Creates a pipe, reads lines until the delimiter is matched.
4. If delimiter was unquoted, `$VAR` and `$?` are expanded in the body.
5. Body is written to the pipe write end; read end stored in `cmd->heredoc_fd`.
6. During execution, if no file input redirect was applied, `heredoc_fd` is dup2'd to stdin.
7. **Ctrl+C during heredoc:** stops reading, returns to prompt, `last_exit = 130`.

---

## 11. Pipes

**Source:** `src/executor/executor_pipeline.c`, `executor_pipeline_steps.c` | **Tester:** `1_pipelines.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `echo hello \| cat` | `hello` | none | 0 |
| `echo a \| cat \| cat` | `a` | none | 0 |
| `true \| false` | (none) | none | **1** (last cmd) |
| `false \| true` | (none) | none | **0** (last cmd) |
| `cd /tmp \| pwd` | (pwd output) | none | 0 |

**How it works:**

- Single command: `execute_single_command()` runs builtins in-parent, externals via fork.
- Multiple commands: `execute_pipeline()` forks all commands as children.
- **Barrier pattern:** A sync pipe (`barrier_write_fd`) blocks all children until the parent closes it, so they start simultaneously. This reduces non-deterministic stderr ordering.
- **Exit code:** Always the exit status of the **last** command in the pipeline.
- **Builtins in pipes:** All commands (including builtins) run in child processes. `cd` in a pipe does not change the parent's directory.

**Non-deterministic failures:** The 5 remaining test failures are due to stderr ordering when multiple child processes write error messages concurrently. These are timing-dependent and not regressions.

---

## 12. Syntax Errors

**Source:** `src/parser/parser_syntax_check.c` | **Tester:** `8_syntax_errors.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `\|` | (none) | `syntax error near unexpected token '\|'` | 2 |
| `\| echo hi` | (none) | `syntax error near unexpected token '\|'` | 2 |
| `echo hi \|` | (none) | `syntax error near unexpected token '\|'` | 2 |
| `echo hi >` | (none) | `syntax error near unexpected token 'newline'` | 2 |
| `>` | (none) | `syntax error near unexpected token 'newline'` | 2 |
| `>> >>` | (none) | `syntax error near unexpected token '>>'` | 2 |
| `cat <<` | (none) | `syntax error near unexpected token 'newline'` | 2 |

**How it works:** `syntax_check()` validates the token list before parsing:

1. Leading pipe: immediate error.
2. Consecutive pipes or trailing pipe: error.
3. Redirection not followed by a `WORD` token: error.
4. `get_token_str()` converts token types to their printable symbols for error messages.

All syntax errors set `last_exit = 2`.

---

## 13. Command Resolution & Execution

**Source:** `src/executor/executor_external.c`, `executor_child.c`, `executor_child_exec.c` | **Tester:** `1_scmds.sh`, `2_path_check.sh`

| Input | stdout | stderr | Exit |
|-------|--------|--------|------|
| `/bin/echo hello` | `hello` | none | 0 |
| `nonexistent_cmd` | (none) | `nonexistent_cmd: command not found` | **127** |
| `/tmp` (directory) | (none) | `/tmp: Is a directory` | **126** |
| Command with no exec permission | (none) | `Permission denied` | **126** |

**How it works:**

1. **Contains `/`:** Use as-is. If it's a directory -> 126. If not executable -> 126. If not found -> 127.
2. **No `/`:** Search `$PATH` directories. `find_command_path()` splits PATH by `:`, tries each `dir/cmd`, checks `is_regular_file()`.
3. **PATH unset but had_path:** Falls back to `/usr/local/bin:/usr/bin:/bin:.`.
4. **PATH unset and no had_path:** Returns NULL -> 127.
5. **PATH empty string:** Searches current directory (returns `ft_strdup(cmd)`).

**Child execution flow:** `execute_in_child()` -> `find_command_path()` -> `execve()`. On failure: `exit_child()` calls `free_all()`, closes standard FDs, then `exit(status)`.

---

## 14. Signals

**Source:** `src/signals/signal_handler.c`, `signal_utils.c`

| Signal | At prompt | During execution |
|--------|-----------|-----------------|
| **Ctrl+C** (SIGINT) | New line, new prompt, `$?` = 130 | Child killed, `$?` = 130 |
| **Ctrl+\\** (SIGQUIT) | Ignored | Child gets core dump, `$?` = 131 |
| **Ctrl+D** (EOF) | Print `exit`, shell exits | Closes stdin for running command |

**How it works:**

- **Interactive mode:** `SIGINT` handler sets `g_signum = SIGINT` and triggers `rl_done`. `SIGQUIT`, `SIGTERM`, `SIGPIPE` are ignored.
- **During execution:** Parent ignores signals (`set_signals_ignore()`). Children get default handlers (`set_signals_default()`).
- **After readline returns:** `check_signal_received()` checks `g_signum`, sets `last_exit = 130` if SIGINT was caught, resets `g_signum`.
- **Heredoc input:** SIGINT during heredoc stops reading, returns to prompt with `last_exit = 130`.

---

## 15. Quote Handling

**Source:** `src/tokenizer/tokenizer_quotes.c`

| Input | stdout | Exit | Notes |
|-------|--------|------|-------|
| `echo 'hello world'` | `hello world` | 0 | Single quotes: literal |
| `echo "hello world"` | `hello world` | 0 | Double quotes: literal + expansion |
| `echo "hello $USER"` | `hello <username>` | 0 | Expansion inside double quotes |
| `echo 'hello $USER'` | `hello $USER` | 0 | No expansion in single quotes |
| `echo "hello 'world'"` | `hello 'world'` | 0 | Single quotes inside double: literal |
| `echo 'hello "world"'` | `hello "world"` | 0 | Double quotes inside single: literal |

**Unclosed quotes:** If input has unclosed quotes, `continuation.c` prompts for more input with `> ` (TTY) or reads from stdin (non-TTY) until the quote is closed.

---

## 16. Exit Code Summary

| Code | Meaning | Example |
|------|---------|---------|
| **0** | Success | Successful command, `exit 0`, `exit 256` (256 % 256) |
| **1** | General / builtin error | `cd` to bad dir, `export` invalid name, `exit 1 2` (too many) |
| **2** | Syntax error / non-numeric exit | Lone pipe, redirect without file, `exit hello` |
| **126** | Not executable | Directory as command, no exec permission |
| **127** | Command not found | Unknown command, `env with_args` |
| **128+N** | Killed by signal N | 130 = SIGINT, 131 = SIGQUIT |

---

## 17. Error Message Format

All error messages follow the pattern:

```
minishell: CONTEXT: MESSAGE
```

Examples:
```
syntax error near unexpected token `|'
minishell: cd: /nonexistent: No such file or directory
minishell: exit: hello: numeric argument required
export: `1bad': not a valid identifier
minishell: nosuchcmd: command not found
minishell: ./dir: Is a directory
nonexistent: No such file or directory
```

**Note:** Some messages (syntax errors, export errors) omit the `minishell:` prefix for historical reasons. Redirection errors use `strerror(errno)` for the message.

---

## 18. Known Bash Deltas

These are intentional or known differences between this minishell and bash:

| Feature | Bash behavior | Minishell behavior | Impact |
|---------|--------------|-------------------|--------|
| `exit hello` exit code | 255 | 2 | Tester EXIT_CODE mismatch |
| `;` separator | Supported | Not implemented (subject scope) | N/A |
| `\` backslash escape | Supported | Not implemented (subject scope) | N/A |
| `&&` / `\|\|` | Supported | Not implemented (bonus scope) | N/A |
| Wildcards `*` | Supported | Not implemented (bonus scope) | N/A |

---

## 19. Not Implemented (Out of Mandatory Scope)

| Feature | Status |
|---------|--------|
| `&&` / `\|\|` (logical operators) | Not implemented (bonus) |
| `;` (command separator) | Not implemented |
| `\` (backslash escape) | Not implemented |
| `*` (wildcards/globbing) | Not implemented (bonus) |
| Subshells `()` | Not implemented (bonus) |

---

## 20. Tester Script Reference

| Script | Content |
|--------|---------|
| `1_builtins_echo.sh` | Echo with flags, quotes, expansion |
| `1_builtins_cd.sh` | CD with HOME, OLDPWD, errors |
| `1_builtins_pwd.sh` | PWD basic and extra args |
| `1_builtins_export.sh` | Export set, append, display, errors |
| `1_builtins_unset.sh` | Unset basic and edge cases |
| `1_builtins_env.sh` | Env display and error cases |
| `1_builtins_exit.sh` | Exit with various arguments |
| `0_compare_parsing.sh` | Parsing comparison with bash |
| `1_pipelines.sh` | Pipes, heredocs in pipes |
| `1_redirs.sh` | Redirections and heredocs |
| `1_scmds.sh` | Simple commands and path |
| `1_variables.sh` | Variable expansion cases |
| `2_path_check.sh` | PATH resolution edge cases |
| `8_syntax_errors.sh` | Syntax error detection |
| `9_go_wild.sh` | env -i and PATH edge cases |
| `10_parsing_hell.sh` | Complex parsing edge cases |

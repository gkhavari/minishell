# Data Model & Function Reference

This document explains **why** we chose the current data structures and lists **every function** in the codebase with a one-line description. Use it for onboarding, refactors, and debugging. See [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) for flow (including Mermaid figures: §0–§8, §11) and [BEHAVIOR.md](BEHAVIOR.md) for tester-facing semantics; **§0** in BEHAVIOR maps topics to architecture sections.

---

## Part 1: Data Model (Structs & Enums)

All types live in **`includes/structs.h`**. The design follows: (1) one representation for each pipeline stage, (2) linked lists for variable-length data, (3) minimal globals.

### 1.1 Token types and parser state

| Type | Definition | Why we use it |
|------|------------|----------------|
| **`t_tokentype`** | `WORD`, `PIPE`, `REDIR_IN`, `REDIR_OUT`, `APPEND`, `HEREDOC` | Token type for lexer output. `REDIR_OUT` covers both `>` and `>\|` (clobber treated as plain redirect). `2>` stderr redirect is **not** a separate token — not implemented in mandatory scope. |
| **`t_state`** | `ST_NORMAL`, `ST_SQUOTE`, `ST_DQUOTE` | Quote context during tokenization. Tells us whether `$` should be expanded (only in double quotes) and when to close a quoted span. |

### 1.2 Token: `t_token`

```c
typedef struct s_token {
    t_tokentype  type;
    char        *value;   // lexeme text (e.g. "echo", ">>", "file.txt")
    int          quoted;  // 1 if from inside quotes (affects expansion / heredoc delim)
    struct s_token *next;
} t_token;
```

| Field | Purpose |
|-------|--------|
| **type** | Drives parser: WORD → argument/redir target; PIPE → new command; REDIR_* → open file / heredoc. |
| **value** | Exact string for the token. Needed for filenames, delimiters, and building `argv`. |
| **quoted** | Heredoc: quoted delimiter means no expansion in body. Expansion: single-quoted segments don’t expand `$VAR`. |
| **next** | Linked list: one pass over tokens, no random access. We only need “current + next” for syntax and parsing. |

**Why a linked list:** Input length is unknown; we emit tokens one-by-one. List is O(1) append and we only walk forward, so no need for a dynamic array.

---

### 1.3 Argument: `t_arg`

```c
typedef struct s_arg {
    char        *value;
    struct s_arg *next;
} t_arg;
```

Stores **one argument** (e.g. one element of `argv`). Commands have a variable number of arguments; we append during parsing and later convert to `char **argv` in `finalize_argv()`.

**Why not `char **args` from the start:** During parsing we add args one by one from tokens; a linked list avoids realloc and keeps `add_word_to_cmd()` simple. We build `argv` once in `argv_build.c` when the command is complete.

---

### 1.4 Redirection: `t_redir`

```c
typedef struct s_redir {
    char         *file;   // filename (heredoc delimiter lives on t_command)
    int           fd;     // dup2 target: STDIN_FILENO, STDOUT_FILENO, or STDERR_FILENO
    int           append; // 1 for >>; 0 for <, >, 2>
    struct s_redir *next;
} t_redir;
```

| Field | Purpose |
|-------|--------|
| **file** | Target filename for `<` `>` `>>` `2>`; heredoc body uses `t_command.heredoc_delim` / pipe. |
| **fd** | Which standard stream this redir replaces (`apply_one_redir` in `executor_utils.c` opens `file` and `dup2`s to `fd`). |
| **append** | For stdout: `>>` uses O_APPEND; otherwise O_TRUNC. |
| **next** | Multiple redirections per command; applied left-to-right. |

**Why a list:** Same as before—order matters; last wins per stream when chained.

**Why `fd` instead of `is_input`:** Supports **stderr** redirection (`2>`) as well as stdin/stdout without extra flags.

---

### 1.5 Command: `t_command`

```c
typedef struct s_command {
    t_arg    *args;          // linked list of argument strings
    char    **argv;         // NULL or args as array (built after parse)
    t_redir  *redirs;       // all redirections in order
    int       heredoc_fd;   // read end of pipe for << content (-1 if none)
    char     *heredoc_delim; // delimiter string for <<
    int       heredoc_quoted;// 1 = no expansion in heredoc body
    int       is_builtin;   // 1 = run in parent, 0 = fork
    struct s_command *next;
} t_command;
```

| Field | Purpose |
|-------|--------|
| **args** | Arguments collected during parse (WORD tokens). |
| **argv** | Built from `args` in `finalize_argv()`; used by executor and builtins. |
| **redirs** | File redirections `<` `>` `>>` `2>`; applied before running the command. |
| **heredoc_fd** | After `process_heredocs()`, read-end of the pipe that feeds heredoc content; -1 if no heredoc. |
| **heredoc_delim** | Delimiter for `<<`; stored here so we can read the body in `process_heredocs()`. |
| **heredoc_quoted** | If delimiter was quoted, we don’t expand variables in the heredoc body. |
| **is_builtin** | Set in `finalize_all_commands()` from `is_builtin(argv[0])`. The executor uses it with **`must_run_in_parent()`**: **`cd` / `export` / `unset` / `exit`** always run in the **parent**; **`echo` / `pwd` / `env`** run in the parent **unless** the command has redirections or a heredoc fd—in that case the builtin path goes through **`execute_external`** (fork) like an external command. |
| **next** | Pipeline: `cmd1 | cmd2 | cmd3` → list of three commands. |

**Why both `args` and `argv`:** Parsing produces `args` incrementally; execution (and `execve`) need `argv`. One conversion step keeps parsing and execution clearly separated.

**Why heredoc in `t_command`:** Heredoc is per-command and read before execution; storing delimiter and fd on the command keeps `process_heredocs()` and `apply_redirections()` simple.

**Why `is_builtin` on the command:** So the executor can branch once (single command vs pipeline, then builtin vs external) without re-resolving the name.

---

### 1.6 Shell: `t_shell`

```c
typedef struct s_shell {
    char     **envp;            // owned copy of environment (modified by export/unset/cd)
    char      *user;            // for prompt (from USER; may be NULL if unset)
    char      *cwd;             // for prompt (from getcwd)
    int        last_exit;       // exit status of last command ($?)
    int        had_path;        // true if PATH existed at shell startup
    int        barrier_write_fd;// pipeline launch barrier FD (-1 if inactive)
    t_token   *tokens;          // result of tokenize_input()
    t_command *commands;        // result of parse_input()
    char      *input;           // current line (owned; freed after tokenize or on reset)
    int        word_quoted;     // internal tokenizer flag: current word is quoted
    int        heredoc_mode;    // internal tokenizer flag: suppress $ expansion in heredoc delim
} t_shell;
```

| Field | Purpose |
|-------|--------|
| **envp** | We own it so `export`/`unset`/`cd` can change it; passed to `execve`. |
| **user, cwd** | Prompt construction; updated by `cd`. |
| **last_exit** | Exit status of last command; used for `$?` and by `exit` with no args. |
| **had_path** | Set in **`init_runtime_fields()`**: whether **`PATH`** was present in the environment **after** dup (used by **`find_command_path()`** and edge cases when **`PATH`** is unset later). |
| **barrier_write_fd** | Reserved for a pipeline launch barrier; **`execute_pipeline()`** currently sets **`shell->barrier_write_fd = -1`** and uses **`sync_fd[0/1] = -1`**. The all-not-found fast path (**`executor_pipeline_not_found.c`**) addresses stderr ordering for the “every stage not found” case without an active barrier. |
| **tokens** | Output of tokenizer; input to parser; freed after parse or on syntax error. |
| **commands** | Output of parser; input to heredoc + executor; freed after execution or on error. |
| **input** | Current line from `readline` (TTY) or `read_line_stdin` in `main.c` (non-TTY); freed in tokenizer or in `reset_shell()`. |
| **word_quoted** | Internal flag set by `mark_word_quoted()` during tokenization; tells `flush_word()` whether the current token came from a quoted span. |
| **heredoc_mode** | Internal flag set by `set_heredoc_mode()`; when active, the tokenizer does not expand `$` in the heredoc delimiter string. |

**Why one shell struct:** Single place for “current line’s” state (tokens, commands, input) and persistent state (env, cwd, last_exit). No global state except `g_signum` for signals.

---

### 1.7 Heredoc context: `t_heredoc_ctx`

```c
typedef struct s_heredoc_ctx {
    t_command  *cmd;        // command that owns this heredoc
    t_shell    *shell;      // shell reference (for expansion, signals)
    int         pipe_fd[2]; // pipe: write end for body, read end becomes cmd->heredoc_fd
    int         expand;     // 1 = expand $VAR in body; 0 = literal (quoted delimiter)
} t_heredoc_ctx;
```

| Field | Purpose |
|-------|--------|
| **cmd** | The command this heredoc belongs to; its `heredoc_fd` is set after reading. |
| **shell** | Needed for `expand_heredoc_line()` (variable values) and signal checking. |
| **pipe_fd** | The heredoc body is written to `pipe_fd[1]`; `pipe_fd[0]` becomes `cmd->heredoc_fd` for execution. |
| **expand** | Determined by `is_quoted_delimiter()`: if the delimiter was quoted (`'EOF'` or `"EOF"`), no expansion. |

**Why a context struct:** `read_heredoc()` and its helpers (`heredoc_read_loop`, `heredoc_interrupted`) need access to the command, shell, pipe, and expansion flag. Bundling them avoids passing 4+ parameters through the call chain (42 Norm limits functions to 4 params).

---

### 1.8 Builtin enum: `t_builtin`

```c
typedef enum e_builtin {
    NOT_BUILTIN = 0, BUILTIN_ECHO, BUILTIN_CD, BUILTIN_PWD,
    BUILTIN_EXPORT, BUILTIN_UNSET, BUILTIN_ENV, BUILTIN_EXIT
} t_builtin;
```

Used by `get_builtin_type()` / `run_builtin()` to dispatch without string comparison in the hot path. Adding a builtin = one new enum value and one case in the dispatcher.

---

### 1.9 Summary diagram

```mermaid
erDiagram
    t_shell ||--o| t_token : "tokens"
    t_shell ||--o| t_command : "commands"
    t_shell ||--o| envp : "envp"
    t_command ||--o| t_arg : "args"
    t_command ||--o| t_redir : "redirs"
    t_command --> argv : "argv (from args)"
    t_heredoc_ctx --> t_command : "cmd"
    t_heredoc_ctx --> t_shell : "shell"
    t_token --> t_token : "next"
    t_arg --> t_arg : "next"
    t_redir --> t_redir : "next"
    t_command --> t_command : "next (pipeline)"
```

---

## Part 2: Function Reference

Functions are grouped by **source file**. Each row: function name, return type / signature summary, and a short description.

### 2.1 Main & init

| File | Function | Description |
|------|----------|-------------|
| **main.c** (src/) | `main(argc, argv, envp)` | Zeros `t_shell`, `init_shell()`, `set_signals_interactive()`, sets `rl_event_hook` on TTY, `shell_loop()`, `rl_clear_history()`, `free_all()`, closes std fds, returns `shell.last_exit`. |
| **main.c** (src/) | `read_line_stdin(shell)` | Non-TTY: read one byte at a time until `\n` or EOF; returns line without newline (static). |
| **main.c** (src/) | `read_input(shell)` | TTY: `build_prompt` + `readline`; else `read_line_stdin`. EOF → 0 (prints `exit` on TTY). After non-NULL line: `check_signal_received` may return -1 (SIGINT). Else 1. (static) |
| **main.c** (src/) | `shell_loop(shell)` | Loop: `check_signal_received`, `read_input`; on 0 break; on -1 continue; if `input[0]` then `process_input`; detect syntax_err (`!commands && last_exit==2`); `reset_shell`; non-TTY + syntax_err → break. (static) |
| **core/init.c** | `process_input(shell)` | `tokenize_input` → `parse_input` → optional `process_heredocs` (on failure sets `last_exit=130` and returns) → `execute_commands` → assigns `last_exit`. |
| **core/init.c** | `init_shell(shell, envp)` | `ft_arrdup` envp, `USER`, `getcwd` → `cwd`, `init_runtime_fields`, optional `update_shlvl` on interactive TTY. |
| **core/init.c** | `get_env_value(envp, key)` | Returns pointer into `envp[i]` at value after `=`, or NULL. |
| **core/init.c** | `build_prompt(shell)` | Builds prompt string for `readline`; caller frees. |
| **core/init_runtime.c** | `init_runtime_fields(shell)` | `ensure_default_envs`, `last_exit=0`, `barrier_write_fd=-1`, null `tokens/commands/input`, `word_quoted`/`heredoc_mode` 0, **`had_path`** from **`PATH`**. |

---

### 2.2 Utils & memory

| File | Function | Description |
|------|----------|-------------|
| **utils/utils.c** | `ft_strcat(dest, src)` | Appends `src` to `dest` in place. |
| **utils/utils.c** | `ft_realloc(ptr, new_size)` | Reallocates buffer; copies min(old_len, new_size-1); frees old. |
| **utils/utils.c** | `ft_arrdup(envp)` | Duplicates `char**` array (for envp). |
| **utils/utils.c** | `msh_calloc(shell, nmemb, size)` | Like calloc; on failure calls `clean_exit(shell, EXIT_FAILURE)`. |
| **utils/utils2.c** | `clean_exit(shell, status)` | `free_all`, close std fds, `_exit(status)` — normal process exit from child or fatal paths. |
| **utils/utils2.c** | `msh_strdup(shell, s)` | `ft_strdup` or `clean_exit` on failure. |

---

### 2.3 Tokenizer (src/tokenizer/)

| File | Function | Description |
|------|----------|-------------|
| **tokenizer/tokenizer.c** | `tokenize_input(shell)` | Main entry: tokenizer_loop over shell->input, then flush_word; frees input. |
| **tokenizer/tokenizer_utils.c** | `flush_word(shell, word, token)` | If *word non-empty, creates a WORD token and appends to *token list; frees *word. |
| **tokenizer/tokenizer_utils.c** | `mark_word_quoted(shell)` | Sets `shell->word_quoted` flag for the current word. |
| **tokenizer/tokenizer_utils.c** | `set_heredoc_mode(shell, mode)` | Sets `shell->heredoc_mode` so `$` is not expanded in delimiter. |
| **tokenizer/tokenizer_utils.c** | `is_heredoc_mode(shell)` | Returns `shell->heredoc_mode` — whether we are parsing a heredoc delimiter. |
| **tokenizer/tokenizer_utils.c** | `append_char(shell, dst, c)` | Appends one char to *dst, reallocating via msh_calloc. |
| **tokenizer/tokenizer_utils2.c** | `add_token(head, new)` | Appends token `new` to list `*head`. |
| **tokenizer/tokenizer_utils2.c** | `new_token(shell, type, value)` | Allocates a token with type and value (value is strduped). |
| **tokenizer/tokenizer_utils2.c** | `process_normal_char(shell, c, i, word)` | Appends char `c` to word and advances `*i`. |
| **tokenizer/tokenizer_handlers.c** | `handle_end_of_string(shell, state, &word)` | At end of input: add history in normal state; on unclosed quote set syntax error and discard partial word. |
| **tokenizer/tokenizer_handlers.c** | `process_quote(shell, c, state)` | Updates quote state for `'` and `"`; returns 1 if char was quote. |
| **tokenizer/tokenizer_handlers.c** | `handle_operator(shell, i, word)` | If input at *i is operator (| < > << >>), flushes word and calls read_operator. |
| **tokenizer/tokenizer_handlers.c** | `handle_whitespace(shell, i, word)` | Skips spaces/tabs; flushes word if non-empty. |
| **tokenizer/tokenizer_handlers.c** | `handle_backslash(shell, i, word, state)` | Handles backslash escape sequences. |
| **tokenizer/tokenizer_quotes.c** | `handle_single_quote(shell, i, word, state)` | Reads single-quoted span (no expansion); appends to word. |
| **tokenizer/tokenizer_quotes.c** | `handle_double_quote(shell, i, word, state)` | Reads double-quoted span; expands `$VAR` and `$?`. |
| **tokenizer/tokenizer_ops.c** | `is_op_char(c)` | Returns 1 if c is `|`, `<`, or `>`. |
| **tokenizer/tokenizer_ops.c** | `read_operator(shell, s, list)` | Parses one operator at s, creates token, appends to list; returns 1 or 2 (chars consumed). |
| **tokenizer/expansion.c** | `expand_var(shell, i)` | Expands one variable at *i ($VAR or $?); advances *i; returns new string (caller frees). |
| **tokenizer/expansion.c** | `handle_variable_expansion(shell, i, word)` | If input at *i is `$` and expandable, expands and appends to word. |
| **tokenizer/expansion.c** | `handle_tilde_expansion(shell, i, word)` | Expands `~` to HOME and appends to word. |
| **tokenizer/expansion_utils.c** | `append_expansion_quoted(word, exp)` | Appends string `exp` to *word (quoted context). |
| **tokenizer/expansion_utils.c** | `append_expansion_unquoted(shell, word, exp, tokens)` | Appends expansion result; may split into multiple WORDs (IFS). |
| **tokenizer/expansion_utils.c** | `handle_empty_unquoted_expansion(shell, start, end, word)` | Handles empty expansion in unquoted context (inserts `MSH_EMPTY_EXPAND_TOKEN` or adjusts for ambiguous redirect). |
| *(continuation support removed — unclosed quotes now emit a syntax error immediately)* |

---

### 2.4 Parser (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/parser.c** | `parse_input(shell)` | Syntax check; on success parse_tokens + finalize_all_commands; on error sets last_exit=2 and frees tokens. |
| **parser/parser.c** | `is_redirection(type)` | Returns 1 if type is REDIR_IN, REDIR_OUT, APPEND, or HEREDOC. (used in parser) |
| **parser/add_token_to_cmd.c** | `add_token_to_command(shell, cmd, token)` | Dispatches token: WORD → add_word_to_cmd; HEREDOC/redirs → set delim or append_redir. Returns 1 (WORD) or 2 (redir/heredoc). |
| **parser/argv_build.c** | `finalize_all_commands(shell, cmd)` | For each command: finalize_argv then set cmd->is_builtin = is_builtin(argv[0]). |
| **parser/argv_build.c** | `finalize_argv(shell, cmd)` | Builds cmd->argv from cmd->args (NULL-terminated array). |
| **parser/parser_syntax_check.c** | `syntax_check(token)` | Validates: no leading/trailing/double pipe; every redir followed by WORD. Returns SYNTAX_OK or SYNTAX_ERR. |
| **parser/parser_syntax_check.c** | `syntax_error(msg)` | Prints "minishell: syntax error near unexpected token 'msg'" to stderr; returns SYNTAX_ERR. |

---

### 2.5 Heredoc (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/heredoc.c** | `process_heredocs(shell)` | Walks commands; if `heredoc_delim`, `read_heredoc`; returns **FAILURE** on interrupt/error. |
| **parser/heredoc.c** | `read_heredoc(cmd, shell, line_no)` | `pipe`, `heredoc_read_loop`: TTY `readline("> ")` else byte-read like `main`; sets `cmd->heredoc_fd`. |
| **parser/heredoc_utils.c** | `is_quoted_delimiter(delim)` | Returns 1 if delimiter is quoted (e.g. `'EOF'` or `"EOF"`) so body is not expanded. |
| **parser/heredoc_utils.c** | `expand_heredoc_line(line, shell)` | Expands `$VAR` and `$?` in line; returns new string (caller frees). |
| **parser/heredoc_warning.c** | `print_heredoc_eof_warning(line_no, delim)` | Bash-style EOF-before-delim warning. |
| **parser/heredoc_warning.c** | `write_heredoc_line(line, fd, expand, shell)` | Writes one line to pipe write end; optional expansion. |

---

### 2.6 Executor (src/executor/)

| File | Function | Description |
|------|----------|-------------|
| **executor/executor.c** | `execute_commands(shell)` | No commands → success; single cmd → empty / `run_builtin_command` / `execute_external`; else `execute_pipeline`. Returns last status. |
| **executor/executor.c** | *(static)* `run_empty_command` | Redirs/heredoc only: backup fds, `apply_redirections`, restore. |
| **executor/executor.c** | *(static)* `run_builtin_command` | If builtin with redirs and **not** `must_run_in_parent` → `execute_external`; else dup/apply/restore and `run_builtin`. |
| **executor/executor.c** | *(static)* `backup_fds` / `restore_fds` | Dup stdin/stdout for builtin redir in parent. |
| **executor/executor_utils.c** | `apply_redirections(cmd)` | Walk `redirs`, open/dup2; then dup `heredoc_fd` to stdin if ≥ 0. |
| **executor/executor_external.c** | `execute_external(cmd, shell)` | Fork; child `set_signals_default`, `apply_redirections`, `execute_in_child`; parent `set_signals_ignore`, `waitpid`, `set_signals_interactive`, `get_child_status`. |
| **executor/executor_external.c** | *(static)* `get_child_status` | `WIFEXITED` / `WIFSIGNALED` → 128+sig; SIGQUIT prints “Quit (core dumped)”. |
| **executor/executor_external.c** | *(static)* `search_in_path` / `build_candidate` | Colon-scan PATH with `stat` (regular file). |
| **executor/executor_external.c** | `find_command_path(cmd, shell)` | **`PATH_MAX`** static buffer; absolute or PATH search; default list if `had_path` and PATH unset. |
| **executor/executor_child_exec.c** | *(static)* `run_builtin_child` | SIGPIPE ignore, `clean_exit(run_builtin(...))`. |
| **executor/executor_child_exec.c** | *(static)* `cmd_not_found` / `check_is_dir` / `handle_exec_error` | Error messages + `clean_exit` 127/126. |
| **executor/executor_child_exec.c** | `execute_in_child(cmd, shell)` | Builtin branch, empty argv, `find_command_path`, `execve` or errors. |
| **executor/executor_child_format.c** | `dprintf_cmd_not_found` (static `$'…'` helper) | one stderr line for not-found, incl. special bytes. |
| **executor/executor_pipeline.c** | *(static)* `wait_children_last` | `waitpid(-1,…)` until **n** children; status of **last_pid** wins. |
| **executor/executor_pipeline.c** | *(static)* `run_pipeline_loop` | Chains `run_pipe_step`, closes trailing `prev_fd`. |
| **executor/executor_pipeline.c** | `execute_pipeline(cmds, shell)` | `sync_fd` inactive (-1); `handle_all_not_found_pipeline` → **127**; ignore signals, loop, wait, restore interactive. |
| **executor/executor_pipeline_steps.c** | *(static)* `setup_child_fds` / `fork_pipeline_cmd` | Pipe wiring between stages. |
| **executor/executor_pipeline_steps.c** | `run_pipe_step(cmd, shell, prev_fd, sync_fd)` | Public wrapper used by `executor_pipeline.c` (declared in `.c` file). |
| **executor/executor_pipeline_not_found.c** | `handle_all_not_found_pipeline(cmds, shell)` | If every stage is simple missing-PATH external (no redir/heredoc), print all “not found” in parent; returns **non-zero** so caller returns **127**. |

---

### 2.7 Builtins & dispatcher (src/builtins/)

| File | Function | Description |
|------|----------|-------------|
| **builtins/builtin_dispatcher.c** | `get_builtin_type(cmd)` | Returns enum (BUILTIN_ECHO, etc.) or NOT_BUILTIN. |
| **builtins/builtin_dispatcher.c** | `is_builtin(cmd)` | Returns 1 if cmd is a builtin name, 0 otherwise. |
| **builtins/builtin_dispatcher.c** | `must_run_in_parent(type)` | True for **cd / export / unset / exit** (must mutate parent shell or exit process). |
| **builtins/builtin_dispatcher.c** | `run_builtin(argv, shell)` | Dispatches to the correct builtin by name; returns builtin return value. |
| **builtins/echo.c** | `builtin_echo(args, shell)` | Prints args to stdout with spaces; handles -n (no newline). Returns 0. |
| **builtins/cd.c** | `builtin_cd(args, shell)` | Changes directory (arg or HOME); updates PWD/OLDPWD in envp; returns 0/1. |
| **builtins/pwd.c** | `builtin_pwd(args, shell)` | Prints current working directory. Returns 0. |
| **builtins/env.c** | `builtin_env(args, shell)` | Prints envp (one KEY=value per line). Returns 0. |
| **builtins/export.c** | `builtin_export(args, shell)` | No args: print declare -x list; with args: add/update env; invalid name → error, return 1. |
| **builtins/export_utils.c** | `is_valid_export_name(name)` | Returns 1 if name is valid for export (letter/underscore start, alnum/_). |
| **builtins/export_utils.c** | `find_export_key_index(shell, key, key_len)` | Finds index of `KEY=value` or bare `KEY` in envp. |
| **builtins/export_utils.c** | `append_export_env(shell, entry)` | Appends one "KEY=value" to shell->envp. |
| **builtins/export_print.c** | `print_sorted_env(shell)` | Prints env in declare -x format (sorted). |
| **builtins/unset.c** | `builtin_unset(args, shell)` | Removes listed vars from envp. Returns 0/1. |
| **builtins/exit.c** | `builtin_exit(args, shell)` | Exits shell: optional numeric status; too many args → error; non-numeric → **2** (bash: 255). Calls free_all. |
| **builtins/exit_utils.c** | `parse_exit_value(str, result)` | Parses a string as a `long long` exit value with overflow detection. Returns 0 on success, 1 on error. |

---

### 2.8 Signals & free

| File | Function | Description |
|------|----------|-------------|
| **signals/signal_handler.c** | `set_signals_default()` | SIGINT/SIGQUIT/SIGPIPE/SIGTERM → **SIG_DFL** (after fork, before exec). |
| **signals/signal_handler.c** | `set_signals_ignore()` | SIGINT/SIGQUIT → **SIG_IGN** (parent while waiting on child/pipeline). |
| **signals/signal_handler.c** | `set_signals_interactive()` | SIGQUIT/SIGTERM/SIGPIPE ignored; SIGINT → **`interactive_sigint_handler`** with **SA_RESTART** and mask **SIGQUIT**; clears **`g_signum`**. |
| **signals/signal_utils.c** | `readline_event_hook()` | If **`g_signum == SIGINT`**: clear readline line, **`rl_done = 1`**. |
| **signals/signal_utils.c** | `check_signal_received(shell)` | If **`g_signum == SIGINT`**: **`last_exit = EXIT_SIGINT` (130)**, clear flag, return 1. |
| **free/free_utils.c** | `free_tokens(token)` | Frees token list and values. |
| **free/free_utils.c** | `free_args(arg)` | Frees `t_arg` list and values. |
| **free/free_runtime.c** | `free_commands(cmd)` | Frees commands: args, argv, redirs, heredoc_delim, closes `heredoc_fd` if set. |
| **free/free_shell.c** | `reset_shell(shell)` | Frees tokens, commands, input; **does not** clear **`last_exit`**, env, cwd. |
| **free/free_shell.c** | `free_all(shell)` | Full teardown including **`envp`**, user, cwd, input, tokens, commands. |

---

## Part 3: Call flow (high level)

Aligned with [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) **§0.4** and **§2**.

```mermaid
flowchart TB
    main[main → init_shell] --> loop[shell_loop]
    loop --> chk[check_signal_received]
    chk --> ri[read_input]
    ri --> gate{input[0]?}
    gate -->|yes| proc[process_input]
    gate -->|no| rs[reset_shell]
    proc --> tok[tokenize_input]
    tok --> par[parse_input]
    par --> hd[process_heredocs]
    hd --> ex[execute_commands]
    ex --> br{single vs pipeline}
    br --> emp[run_empty / run_builtin_command / execute_external]
    br --> pl[execute_pipeline → run_pipe_step]
    emp --> ich[execute_in_child / clean_exit in child]
    pl --> ich
    proc --> rs
    rs --> loop
```

---

## Related docs

| Document | Content |
|----------|---------|
| [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) | Pipeline stages, signals, source layout, testing. |
| [BEHAVIOR.md](BEHAVIOR.md) | Input/output semantics, exit codes, builtin behavior. |

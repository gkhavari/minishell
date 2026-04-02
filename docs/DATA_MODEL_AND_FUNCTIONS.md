# Data Model & Function Reference

This document explains **why** we chose the current data structures and lists **every function** in the codebase with a one-line description. Use it for onboarding, refactors, and debugging. See [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) for flow (including Mermaid figures: §0–§8, §11) and [BEHAVIOR.md](BEHAVIOR.md) for tester-facing semantics; **§0** in BEHAVIOR maps topics to architecture sections.

---

## Part 1: Data Model (Structs & Enums)

All types live in **`includes/structs.h`**. The design follows: (1) one representation for each pipeline stage, (2) linked lists for variable-length data, (3) minimal globals.

### 1.1 Token types and parser state

| Type | Definition | Why we use it |
|------|------------|----------------|
| **`t_tokentype`** | `WORD`, `PIPE`, `REDIR_IN`, `REDIR_OUT`, `APPEND`, `HEREDOC` | Token type for tokenizer output. `REDIR_OUT` covers both `>` and `>\|` (clobber treated as plain redirect). `2>` stderr redirect is **not** a separate token — not implemented in mandatory scope. |
| **`t_state`** | `ST_NORMAL`, `ST_SQUOTE`, `ST_DQUOTE` | Quote context during tokenization. Tells us whether `$` should be expanded (only in double quotes) and when to close a quoted span. |

### 1.2 Token: `t_token`

```c
typedef struct s_token {
    t_tokentype  type;
    char        *value;   // lexeme text (e.g. "echo", ">>", "file.txt")
    int          quoted;  // 1 if from inside quotes (affects expansion / heredoc delim)
} t_token;
```

| Field | Purpose |
|-------|--------|
| **type** | Drives parser: WORD → argument/redir target; PIPE → new command; REDIR_* → open file / heredoc. |
| **value** | Exact string for the token. Needed for filenames, delimiters, and building `argv`. |
| **quoted** | Heredoc: quoted delimiter means no expansion in body. Expansion: single-quoted segments don’t expand `$VAR`. |

**Chain:** `t_shell.tokens` is a **`t_list *`** (`libft`). Each node’s **`content`** points to a **`t_token`**. Append uses **`ft_lstadd_back`**; free uses **`ft_lstclear`** with a delete that frees `value` and the `t_token`.

---

### 1.3 Argument: `t_arg`

```c
typedef struct s_arg {
    char        *value;
} t_arg;
```

Stores **one argument** (e.g. one element of `argv`). **`t_command.args`** is a **`t_list *`** of nodes whose **`content`** is **`t_arg *`**. We build **`argv`** once in **`argv_build.c`** (`ft_lstsize` / walk).

---

### 1.4 Redirection: `t_redir`

```c
typedef struct s_redir {
    char         *file;   // filename (heredoc delimiter lives on t_command)
    int           fd;     // dup2 target: STDIN_FILENO, STDOUT_FILENO, …
    int           append; // 1 for >>; 0 for <, >
} t_redir;
```

| Field | Purpose |
|-------|--------|
| **file** | Target filename for `<` `>` `>>`; heredoc body uses `t_command.heredoc_delim` / pipe. |
| **fd** | Which standard stream this redir replaces (`rdr_one` in `executor/exe_redir.c` opens `file` and `dup2`s to `fd`). |
| **append** | For stdout: `>>` uses O_APPEND; otherwise O_TRUNC. |

**Chain:** **`t_command.redirs`** is **`t_list *`** (`content` → **`t_redir *`**).

**Why a list:** Same as before—order matters; last wins per stream when chained.

**Why `fd` instead of `is_input`:** Supports **stderr** redirection (`2>`) as well as stdin/stdout without extra flags.

---

### 1.5 Command: `t_command`

```c
typedef struct s_command {
    t_list   *args;           // t_arg * per node
    char    **argv;          // NULL or args as array (built after parse)
    t_list   *redirs;        // t_redir * per node
    int       heredoc_fd;    // read end of pipe for << (-1 if none)
    char     *heredoc_delim;
    int       heredoc_quoted;
    int       is_builtin;
} t_command;
```

| Field | Purpose |
|-------|--------|
| **args** | Arguments collected during parse (WORD tokens). |
| **argv** | Built from `args` in `finalize_argv()`; used by executor and builtins. |
| **redirs** | File redirections `<` `>` `>>`; applied before running the command. |
| **heredoc_fd** | After `process_heredocs()`, read-end of the pipe that feeds heredoc content; -1 if no heredoc. |
| **heredoc_delim** | Delimiter for `<<`; stored here so we can read the body in `process_heredocs()`. |
| **heredoc_quoted** | If delimiter was quoted, we don’t expand variables in the heredoc body. |
| **is_builtin** | Set in `finalize_all_commands()` from `(get_builtin_type(argv[0]) != B_NONE)`. In **`exe.c`**, **`run_single_builtin`** treats **`cd` / `export` / `unset` / `exit`** as parent-only; **`echo` / `pwd` / `env`** run in the parent **unless** the command has redirections or a heredoc fd—in that case the builtin path goes through **`run_external`** (fork) like an external command. |

**Pipeline:** **`t_shell.commands`** is **`t_list *`** (`content` → **`t_command *`**).

**Why both `args` and `argv`:** Parsing produces `args` incrementally; execution (and `execve`) need `argv`. One conversion step keeps parsing and execution clearly separated.

**Why heredoc in `t_command`:** Heredoc is per-command and read before execution; storing delimiter and fd on the command keeps `process_heredocs()` and `apply_redirs()` simple.

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
    t_list    *tokens;          // tokenize_input(): content = t_token *
    t_list    *commands;        // parse_input(): content = t_command *
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
| **had_path** | Set in **`init_runtime_fields()`**: whether **`PATH`** was present in the environment **after** dup (used by **`resolve_cmd_path()`** and edge cases when **`PATH`** is unset later). |
| **barrier_write_fd** | Reserved for a pipeline launch barrier; **`run_pipeline()`** sets **`shell->barrier_write_fd = -1`** and uses **`sync_fd[0/1] = -1`** (inactive). The all-not-found fast path (**`pipeline_all_nf()`** in **`exe_pipeline_nf.c`**) addresses stderr ordering for the “every stage not found” case without an active barrier. |
| **tokens** | Output of tokenizer; input to parser; freed after parse or on syntax error. |
| **commands** | Output of parser; input to heredoc + executor; freed after execution or on error. |
| **input** | Current line from `readline` (TTY) or `ft_read_stdin_line` via **`shell_repl.c`** (non-TTY); freed in tokenizer or in `reset_shell()`. |
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
    B_NONE = 0, B_ECHO, B_CD, B_PWD,
    B_EXPORT, B_UNSET, B_ENV, B_EXIT, B_COUNT
} t_builtin;

typedef struct s_builtin_reg {
    const char *name;
    int (*run)(char **, t_shell *);
} t_builtin_reg;
```

Used by `get_builtin_type()` / `run_builtin()`. `B_COUNT` is the exclusive end (table length = `B_COUNT - B_ECHO`). `t_builtin_reg` lives in `structs.h` (Norm: no struct/typedef in `.c`). The registry is a function-local `static const t_builtin_reg tab[]` inside `builtin_registry()` in `builtin_dispatcher.c` (no file-scope globals). Adding a builtin = new enum value before `B_COUNT`, one registry row, and implement `builtin_*`.

---

### 1.9 Summary diagram

```mermaid
erDiagram
    t_shell ||--o| t_list : "tokens"
    t_shell ||--o| t_list : "commands"
    t_shell ||--o| envp : "envp"
    t_list ||--o| t_token : "content"
    t_list ||--o| t_command : "content"
    t_command ||--o| t_list : "args"
    t_command ||--o| t_list : "redirs"
    t_list ||--o| t_arg : "args.content"
    t_list ||--o| t_redir : "redirs.content"
    t_command --> argv : "argv (from args)"
    t_heredoc_ctx --> t_command : "cmd"
    t_heredoc_ctx --> t_shell : "shell"
```

---

## Part 2: Function Reference

Functions are grouped by **source file**. Each row: function name, return type / signature summary, and a short description.

### 2.1 Main, REPL & init

| File | Function | Description |
|------|----------|-------------|
| **main.c** (src/) | `main(argc, argv, envp)` | Zeros `t_shell`, `init_shell()`, `set_signals_interactive()`, sets `rl_event_hook` on TTY, **`shell_loop()`**, `rl_clear_history()`, `free_all()`, closes std fds, returns `shell.last_exit`. |
| **core/shell_repl.c** | `shell_loop(shell)` | REPL: `check_signal_received`, `read_input`; **`RL_EOF`** break; **`RL_SIG`** continue; read-path **`OOM`**: **`last_exit` FAILURE, `reset_shell`, break** (exit toward **`main`**); else **`repl_after_read`** (`process_input` if `input[0]`, syntax_err / `reset_shell`, non-TTY break on **`XSYN`**). |
| **core/shell_repl.c** | *(static)* `read_input(shell)` | TTY: `build_prompt` + `readline`; else **`read_line_stdin`** → **`ft_read_stdin_line(..., 0)`**. Returns **`RL_LN`**, **`RL_EOF`**, **`RL_SIG`**, or **`OOM`**. |
| **core/shell_repl.c** | *(static)* `build_prompt(shell)` | Prompt string for TTY `readline`; caller frees. |
| **core/shell_repl.c** | *(static)* `read_line_stdin(shell, out)` | Wrapper: `ft_read_stdin_line(shell, out, 0)`. |
| **core/shell_repl.c** | *(static)* `repl_after_read(shell)` | If `input[0]` → `process_input`; syntax_err when `!commands && last_exit == XSYN`; `reset_shell`; returns 1 to break on non-TTY + syntax_err. |
| **utils/read_stdin_line.c** | `ft_read_stdin_line(shell, line, set_shell_oom_on_fail)` | Non-TTY: byte-read until `\n` or EOF; **`RL_LN`** / **`RL_EOF`** / **`OOM`**; optional `shell->oom` on append failure. |
| **core/init.c** | `process_input(shell)` | `tokenize_input` → `parse_input` → optional `process_heredocs` (on failure: SIGINT → `EXIT_SIGINT`, else `FAILURE`) → `run_commands` → assigns `last_exit`. |
| **core/init.c** | `init_shell(shell, envp)` | `init_shell_identity` → `init_runtime_fields`; interactive TTY → `update_shlvl`. |
| **core/init_utils.c** | `init_shell_identity(shell, envp)` | `ft_arrdup` envp, `USER`, `getcwd` (fallback `/`); on fatal alloc/OOM path `ft_dprintf` + `FAILURE` via `clean_exit`. |
| **core/init_utils.c** | `init_runtime_fields(shell)` | `ensure_default_envs`, `last_exit=SUCCESS`, `barrier_write_fd=-1`, null `tokens/commands/input`, `word_quoted`/`heredoc_mode` 0, **`had_path`** from **`PATH`**. |
| **core/init_utils.c** | `get_env_value(envp, key)` | Returns pointer into `envp[i]` at value after `=`, or NULL. |

---

### 2.2 Utils & memory

| File | Function | Description |
|------|----------|-------------|
| **utils/ft_strcat.c** | `ft_strcat(dest, src)` | Appends `src` to `dest` in place. |
| **utils/ft_realloc.c** | `ft_realloc(ptr, new_size)` | Reallocates buffer; copies min(old_len, new_size-1); frees old. |
| **utils/ft_arrdup.c** | `ft_arrdup(envp)` | Duplicates `char**` array (for envp). |
| **utils/msh_string.c** | `msh_is_blank(c, ifs_mode)` | `ifs_mode` 0: space+tab (tokenizer); 1: space+tab+newline (default IFS; not full `ft_isspace`). |
| **utils/msh_string.c** | `msh_env_var_body_span`, `msh_is_dollar_var_leader`, … | Shared `$NAME` tail and heredoc `$` leader checks (uses `ft_isalnum` / `ft_isalpha`). |
| **free/free_exit.c** | `clean_exit_before_readline(shell, status)` | `free_all`, close std fds, `exit` — init OOM before any `readline()` (no `rl_clear_history`). |
| **free/free_exit.c** | `clean_exit(shell, status)` | `free_all`, `rl_clear_history`, close std fds, `exit` — child / post-readline fatal paths. |

---

### 2.3 Tokenizer (src/tokenizer/)

| File | Function | Description |
|------|----------|-------------|
| **tokenizer/tokenizer.c** | `tokenize_input(shell)` | Calls **`tokenizer_loop`** on **`shell->input`**, then **`tokenizer_end`** (static): **`flush_word`** in **`ST_NORMAL`** or discard tokens on unclosed quote; frees **`shell->input`**. |
| **tokenizer/tokenizer_loop.c** | `tokenizer_loop(shell, i, state, word)` | Main character loop: quotes, expansion, backslash, operators, whitespace, normal char; returns **`OOM`** on allocation failure. Internal helpers include **`tok_call_handler`**, **`tok_try_expand_unquoted`**, **`tok_run_secondary`**. |
| **tokenizer/tokenizer_utils.c** | `flush_word(shell, word, tokens)` | If *word non-empty, creates a WORD token and `ft_lstadd_back` on `*tokens`; frees *word. |
| **tokenizer/tokenizer_utils.c** | `add_token(head, new_tok)` | `ft_lstnew` + `ft_lstadd_back`; on failure frees `new_tok`. |
| **tokenizer/tokenizer_utils.c** | `new_token(shell, type, value)` | Allocates `t_token` with `ft_strdup(value)`. |
| **tokenizer/tokenizer_utils.c** | `append_char(shell, dst, c)` | Appends one char to *dst via `ft_realloc`. |
| **tokenizer/tokenizer_utils.c** | `process_normal_char(shell, c, i, word)` | Appends char `c` to word and advances `*i`. |
| **tokenizer/tokenizer_handlers.c** | `handle_end_of_string(shell, state, &word)` | At end of input: add history in normal state; on unclosed quote set syntax error and discard partial word. |
| **tokenizer/tokenizer_handlers.c** | `process_quote(shell, c, state)` | Updates quote state for `'` and `"`; returns 1 if char was quote. |
| **tokenizer/tokenizer_handlers.c** | `handle_operator(shell, i, word)` | If input at *i is operator (| < > << >>), flushes word and calls read_operator. |
| **tokenizer/tokenizer_handlers.c** | `handle_whitespace(shell, i, word)` | If `msh_is_blank(..., 0)`, flushes word and advances `*i`. |
| **tokenizer/tokenizer_handlers.c** | `handle_backslash(shell, i, word, state)` | Handles backslash escape sequences. |
| **tokenizer/tokenizer_quotes.c** | `handle_single_quote(shell, i, word, state)` | Reads single-quoted span (no expansion); appends to word. |
| **tokenizer/tokenizer_quotes.c** | `handle_double_quote(shell, i, word, state)` | Reads double-quoted span; expands `$VAR` and `$?`. |
| **tokenizer/tokenizer_ops.c** | `is_op_char(c)` | Returns 1 if c is `|`, `<`, or `>`. |
| **tokenizer/tokenizer_ops.c** | `read_operator(shell, s, list)` | Parses one operator at s, `add_token` to `t_list **list`; returns chars consumed or `OOM`. |
| **tokenizer/expansion.c** | `expand_var(shell, i)` | Expands one variable at *i ($VAR or $?); advances *i; returns new string (caller frees). |
| **tokenizer/expansion.c** | `handle_variable_expansion(shell, i, word)` | If input at *i is `$` and expandable, expands and appends to word. |
| **tokenizer/expansion.c** | `handle_tilde_expansion(shell, i, word)` | Expands `~` to HOME and appends to word. |
| **tokenizer/expansion_utils.c** | `append_expansion_quoted(word, exp)` | Appends string `exp` to *word (quoted context). |
| **tokenizer/expansion_utils.c** | `append_expansion_unquoted(shell, word, exp, tokens)` | Appends expansion result; may split into multiple WORDs (IFS). |
| **tokenizer/expansion_utils.c** | `handle_empty_unquoted_expansion(shell, start, end, word)` | Handles empty expansion in unquoted context (inserts `EMPTY_EXPAND` or adjusts for ambiguous redirect). |
| *(continuation support removed — unclosed quotes now emit a syntax error immediately)* |

---

### 2.4 Parser (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/parser.c** | `parse_input(shell)` | `syntax_check`; `build_command_list` + `finalize_all_commands`; frees token list; **`OOM`** from finalize sets **`shell->oom`**, frees commands. |
| **parser/parser_build.c** | `build_command_list(shell, tokens)` | Walks tokens into `t_list` of `t_command *`; **`PARSE_ERR`** → drop partial pipeline; **`OOM`** → **`shell->oom`**, drop pipeline. |
| **parser/parser_redir.c** | `parse_redir_token_pair(cmd, tok_node)` | Redir + WORD → `cmd->redirs`; **`PARSE_ERR`** if missing word; **`OOM`** if malloc fails. |
| **parser/add_token_to_cmd.c** | `add_token_to_command(...)` | WORD/HEREDOC/redir dispatch; **`PARSE_ERR`** structural; **`OOM`** on allocation failure. |
| **parser/argv_build.c** | `finalize_all_commands(shell, cmd_list)` | Per command: `finalize_argv`; returns **`OOM`** on allocation failure (else **0**). |
| **parser/argv_build.c** | `finalize_argv(shell, cmd)` | Builds cmd->argv from cmd->args (NULL-terminated array). |
| **parser/parser_syntax_check.c** | `syntax_check(lst)` | Walks token `t_list`; validates pipes and redir+WORD. |
| **parser/parser_syntax_check.c** | `syntax_error(msg)` | Prints "minishell: syntax error near unexpected token 'msg'" to stderr; returns **ERR** (same value as **FAILURE**). |

---

### 2.5 Heredoc (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/parser.c** | `process_heredocs(shell)` | Walks commands; if `heredoc_delim`, `read_heredoc`; returns **FAILURE** on interrupt/error. |
| **parser/heredoc_input.c** | `heredoc_read_line(shell)` | TTY `readline("> ")` else `ft_read_stdin_line(..., 1)` (sets `shell->oom` on OOM). |
| **parser/heredoc_input.c** | `print_heredoc_eof_warning(line_no, delim)` | Bash-style EOF-before-delim warning to stderr. |
| **parser/heredoc_input.c** | `write_heredoc_line(line, fd, expand, shell)` | Writes one line to pipe write end; optional expansion. |
| **parser/heredoc.c** | `read_heredoc(cmd, shell, line_no)` | `pipe`, loop via **`heredoc_read_one`** / **`heredoc_read_line`**; sets `cmd->heredoc_fd`. |
| **parser/heredoc_utils.c** | `is_quoted_delimiter(delim)` | Returns 1 if delimiter is quoted (e.g. `'EOF'` or `"EOF"`) so body is not expanded. |
| **parser/heredoc_utils.c** | `expand_heredoc_line(line, shell)` | Expands `$VAR` and `$?` in line; returns new string (caller frees). |

---

### 2.6 Executor (src/executor/, `exe_*` files; public API unprefixed)

| File | Function | Description |
|------|----------|-------------|
| **executor/exe.c** | `run_commands(shell)` | No commands → success; single cmd → `run_empty_command` / `run_single_builtin` / `run_external`; else `run_pipeline`. Returns last status. |
| **executor/exe.c** | *(static)* `run_empty_command` | Redirs/heredoc only: `backup_stdio_fds`, `apply_redirs`, `restore_stdio_fds`. |
| **executor/exe.c** | *(static)* `run_single_builtin` | Parent-only for **cd / export / unset / exit**; if another builtin has redirs/heredoc fd → `run_external`; else dup/apply/restore and `run_builtin`. |
| **executor/exe.c** | *(static)* `backup_stdio_fds` / `restore_stdio_fds` | Dup stdin/stdout for builtin redir in parent. |
| **executor/exe_redir.c** | `apply_redirs(cmd)` | Walk `redirs` via `apply_one_redir` (`apply_input_redir` / `apply_output_redir`); then dup `heredoc_fd` to stdin if ≥ 0. |
| **executor/exe_external.c** | `run_external(cmd, shell)` | Fork; child `set_signals_default`, `apply_redirs`, `run_in_child`; parent `set_signals_ignore`, `waitpid`, `set_signals_interactive`, `status_from_child_wait`. |
| **executor/exe_external.c** | *(static)* `status_from_child_wait` | `WIFEXITED` → `WEXITSTATUS`; `WIFSIGNALED` → `EXIT_STATUS_FROM_SIGNAL(WTERMSIG)`; SIGQUIT prints “Quit (core dumped)”. |
| **executor/exe_external.c** | *(static)* `scan_path_for_command` / `build_path_candidate` | Colon-scan PATH with `stat` (regular file). |
| **executor/exe_external.c** | `resolve_cmd_path(cmd, shell)` | **`PATH_MAX`** static buffer; absolute or PATH search; default list if `had_path` and PATH unset. |
| **executor/exe_child.c** | *(static)* `run_builtin_in_child` | SIGPIPE ignore, `clean_exit(run_builtin(...))`. |
| **executor/exe_child.c** | *(static)* `child_exit_not_found` / `child_abort_with_message` | Error messages + `clean_exit` with `EXIT_CMD_NOT_FOUND` / `EXIT_CMD_CANNOT_EXECUTE`. |
| **executor/exe_child.c** | `run_in_child(cmd, shell)` | Builtin branch, empty argv, `resolve_cmd_path`, `execve` or errors. |
| **executor/exe_pipeline_nf.c** | *(static)* `is_simple_not_found_command` | Predicate: simple missing-PATH external, no redir/heredoc, no `/` in argv0. |
| **executor/exe_pipeline_nf.c** | `pipeline_all_nf(cmds, shell)` | If every stage matches **`is_simple_not_found_command`**, print all “not found” via **`put_cmd_not_found`** in parent; returns **TRUE** so **`run_pipeline`** returns **`EXIT_CMD_NOT_FOUND`**. |
| **executor/exe_not_found.c** | `put_cmd_not_found` (+ static `format_not_found_name`, `needs_dollar_quotes`, `append_escaped_char`, `fill_dollar_quoted_name`) | One stderr line for not-found (`$'…'` when name has control bytes). |
| **executor/exe_pipeline.c** | *(static)* `wait_for_pipeline_children` | `waitpid(-1,…)` until **n** children; status of **last_pid** wins (`update_last_status_from_wait`). |
| **executor/exe_pipeline.c** | *(static)* `spawn_pipeline_children` | Chains `pipe_step`, closes trailing `prev_fd`. |
| **executor/exe_pipeline.c** | `run_pipeline(cmds, shell)` | `sync_fd` inactive (-1); `pipeline_all_nf` → **`EXIT_CMD_NOT_FOUND`**; ignore signals, loop, wait, restore interactive. |
| **executor/exe_pipe_step.c** | *(static)* `setup_pipeline_child_fds` / `fork_pipeline_child` / `advance_prev_pipe_fd` | Pipe wiring between stages. |
| **executor/exe_pipe_step.c** | `pipe_step(cmd, shell, prev_fd, sync_fd)` | One pipeline segment; used by `exe_pipeline.c`. |

---

### 2.7 Builtins & dispatcher (src/builtins/)

| File | Function | Description |
|------|----------|-------------|
| **builtins/builtin_dispatcher.c** | `get_builtin_type(cmd)` | Returns enum (`B_ECHO`, etc.) or `B_NONE`; `argv_build.c` uses `!= B_NONE` to set `cmd->is_builtin`. |
| **builtins/builtin_dispatcher.c** | `run_builtin(argv, shell)` | Looks up `argv[0]` and calls the matching `builtin_*` via the static registry inside `builtin_registry()`. |
| **builtins/echo.c** | `builtin_echo(args, shell)` | Prints args to stdout with spaces; handles -n (no newline). Returns 0. |
| **builtins/cd.c** | `builtin_cd(args, shell)` | Changes directory (arg or HOME); updates PWD/OLDPWD in envp; returns 0/1. |
| **builtins/pwd.c** | `builtin_pwd(args, shell)` | Prints current working directory. Returns 0. |
| **builtins/env.c** | `builtin_env(args, shell)` | Prints envp (one KEY=value per line). Returns 0. |
| **builtins/export.c** | `builtin_export(args, shell)` | No args: print declare -x list; with args: add/update env; invalid name → error, return 1. |
| **builtins/export_utils.c** | `export_invalid_identifier_err(arg)` | Prints one `export: \`arg': not a valid identifier` line to stderr. |
| **builtins/export_utils.c** | `is_valid_export_name(name)` | Returns 1 if name is valid for export (letter/underscore start, alnum/_). |
| **builtins/export_utils.c** | `find_export_key_index(shell, key, key_len)` | Finds index of `KEY=value` or bare `KEY` in envp. |
| **builtins/export_utils.c** | `append_export_env(shell, entry)` | Appends one "KEY=value" to shell->envp. |
| **builtins/export_print.c** | `print_sorted_env(shell)` | Prints env in declare -x format (sorted). |
| **builtins/unset.c** | `builtin_unset(args, shell)` | Removes listed vars from envp. Returns 0/1. |
| **builtins/exit.c** | `builtin_exit(args, shell)` | Exits shell: optional numeric status; too many args → **`FAILURE`**; non-numeric → **`EXIT_SYNTAX_ERROR`** (bash: 255). Calls `clean_exit` / free_all. |
| **builtins/exit_utils.c** | `parse_exit_value(str, result)` | Parses a `long long` exit value with overflow checks; skips leading/trailing bytes where `ft_isspace` is true. Returns 1 on success, 0 on error. |

---

### 2.8 Signals & free

| File | Function | Description |
|------|----------|-------------|
| **signals/signal_handler.c** | `set_signals_default()` | SIGINT/SIGQUIT/SIGPIPE/SIGTERM → **SIG_DFL** (after fork, before exec). |
| **signals/signal_handler.c** | `set_signals_ignore()` | SIGINT/SIGQUIT → **SIG_IGN** (parent while waiting on child/pipeline). |
| **signals/signal_handler.c** | `set_signals_interactive()` | SIGQUIT/SIGTERM/SIGPIPE ignored; SIGINT → **`interactive_sigint_handler`** with **SA_RESTART** and mask **SIGQUIT**; clears **`g_signum`**. |
| **signals/signal_utils.c** | `readline_event_hook()` | If **`g_signum == SIGINT`**: clear readline line, **`rl_done = 1`**. |
| **signals/signal_utils.c** | `check_signal_received(shell)` | If **`g_signum == SIGINT`**: **`last_exit = EXIT_SIGINT`** (`EXIT_STATUS_FROM_SIGNAL(SIGINT)`, usually 130), clear flag, return 1. |
| **free/free_utils.c** | `free_tokens(&lst)` | `ft_lstclear` on token list; each `content` is `t_token *`. |
| **free/free_utils.c** | `free_args(&lst)` | `ft_lstclear` on arg list; each `content` is `t_arg *`. |
| **free/free_runtime.c** | `free_commands(&lst)` | `ft_lstclear` on command list; each payload is one `t_command` (args/redirs as lists). |
| **free/free_shell.c** | `free_tokenize(shell, word)` | On tokenizer OOM: frees **`word`**, partial tokens, current **`input`**; sets **`last_exit`**. Caller returns **`OOM`**; does not set **`shell->oom`**. |
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
    hd --> ex[run_commands]
    ex --> br{single vs pipeline}
    br --> emp[run_empty_command / run_single_builtin / run_external]
    br --> pl[run_pipeline → pipe_step]
    emp --> ich[run_in_child / clean_exit in child]
    pl --> ich
    proc --> rs
    rs --> loop
```

### 3.1 Algorithm choices (defense notes)

- **Single-pass tokenizer loop:** character-by-character state machine with
  explicit quote states (`ST_NORMAL`, `ST_SQUOTE`, `ST_DQUOTE`) keeps logic
  deterministic and easy to debug.
- **Token-first parser:** syntax validation happens on token stream before
  command build, so executor never receives structurally invalid pipelines.
- **Linked-list command model:** variable pipeline length and redirection count
  are handled without fixed-size arrays, which fits shell input naturally.
- **Two-phase command finalize:** parse collects `args`; finalize builds
  `argv[]`. This clean boundary keeps parse logic and exec formatting separate.
- **Streaming pipeline spawn + wait:** parent builds one stage at a time and
  reaps with `waitpid(-1, ...)`, tracking last command status for bash parity.
- **Parent-only stateful builtins:** preserves shell state consistency while
  still allowing non-stateful builtins to run through external path when needed.

---

## Related docs

| Document | Content |
|----------|---------|
| [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) | Pipeline stages, signals, source layout, testing. |
| [BEHAVIOR.md](BEHAVIOR.md) | Input/output semantics, exit codes, builtin behavior. |
| [`includes/defines.h`](../includes/defines.h) | **`OK` / `ERR`**, **`TOK_N` / `TOK_Y`** (tokenizer handler: not handled / handled; **`TOK_NO` / `TOK_YES`**, **`LEX_NO` / `LEX_YES`** aliases), **`RL_LN` / `RL_EOF` / `RL_SIG`**, **`XSYN`**, **`XNF` / `XNX`**, **`EXIT_CMD_*`**, **`EXIT_STATUS_FROM_SIGNAL`**, **`EXIT_SIGINT`**, **`OOM`**, **`PR_ERR`**, other parser sentinels; long names kept as aliases where defined. |

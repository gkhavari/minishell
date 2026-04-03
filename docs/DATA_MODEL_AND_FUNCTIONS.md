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
    int           fd;     // dup2 target: STDIN_FILENO or STDOUT_FILENO (mandatory scope)
    int           append; // 1 for >>; 0 for <, >
} t_redir;
```

| Field | Purpose |
|-------|--------|
| **file** | Target filename for `<` `>` `>>`; heredoc body uses `t_command.hd_delim` / pipe. |
| **fd** | Which standard stream this redir replaces (`apply_one_redir` in `executor/msh_executor_apply_redirects.c` opens `file` and `dup2`s to `fd`). |
| **append** | For stdout: `>>` uses O_APPEND; otherwise O_TRUNC. |

**Chain:** **`t_command.redirs`** is **`t_list *`** (`content` → **`t_redir *`**).

**Why a list:** Same as before—order matters; last wins per stream when chained.

**Why `fd` instead of `is_input`:** Holds the **`dup2`** target (**`STDIN_FILENO`** or **`STDOUT_FILENO`** in mandatory scope). **`2>`** stderr redirection is **not** produced by the tokenizer/parser; see [BEHAVIOR.md](BEHAVIOR.md) §6.

---

### 1.5 Command: `t_command`

```c
typedef struct s_command {
    t_list   *args;           // t_arg * per node
    char    **argv;          // NULL or args as array (built after parse)
    t_list   *redirs;        // t_redir * per node
    int       hd_fd;    // read end of pipe for << (-1 if none)
    char     *hd_delim;
    int       hd_quoted;
    int       stdin_last;    // STDIN_LAST_* — last < vs << in source (bash order)
    int       is_builtin;
} t_command;
```

| Field | Purpose |
|-------|--------|
| **args** | Arguments collected during parse (WORD tokens). |
| **argv** | Built from `args` in `finalize_argv()`; used by executor and builtins. |
| **redirs** | File redirections `<` `>` `>>`; applied before running the command. |
| **hd_fd** | After `process_heredocs()`, read-end of the pipe that feeds heredoc content; -1 if no heredoc. |
| **hd_delim** | Delimiter for `<<`; stored here so we can read the body in `process_heredocs()`. |
| **hd_quoted** | If delimiter was quoted, we don’t expand variables in the heredoc body. |
| **stdin_last** | **`STDIN_LAST_NONE`**, **`STDIN_LAST_HD`**, or **`STDIN_LAST_FILE`** (`defines.h`): updated while parsing so **`apply_redirs`** matches bash (last `<` vs `<<` in source wins stdin). |
| **is_builtin** | Set in `finalize_cmds()` from `(get_builtin_type(argv[0]) != B_NONE)`. In **`msh_executor_run_commands.c`**, **`run_single_builtin`** treats **`cd` / `export` / `unset` / `exit`** as parent-only; **`echo` / `pwd` / `env`** run in the parent **unless** the command has redirections or a heredoc fd—in that case the builtin path goes through **`run_external`** (fork) like an external command. |

**Pipeline:** **`t_shell.cmds`** is **`t_list *`** (`content` → **`t_command *`**).

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
    t_list    *cmds;        // parse_input(): content = t_command *
    char      *input;           // current line (owned; freed after tokenize or on reset)
    int        word_quoted;     // internal tokenizer flag: current word is quoted
    int        hd_mod;    // internal tokenizer flag: suppress $ expansion in heredoc delim
    int        oom;             // set when an allocation fails mid-parse; checked by callers
} t_shell;
```

| Field | Purpose |
|-------|--------|
| **envp** | We own it so `export`/`unset`/`cd` can change it; passed to `execve`. |
| **user, cwd** | Prompt construction; updated by `cd`. |
| **last_exit** | Exit status of last command; used for `$?` and by `exit` with no args. |
| **had_path** | Set in **`init_runtime_fields()`**: whether **`PATH`** was present in the environment **after** dup (used by **`resolve_cmd_path()`** and edge cases when **`PATH`** is unset later). |
| **barrier_write_fd** | Reserved for a pipeline launch barrier; **`run_pip()`** sets **`shell->barrier_write_fd = -1`** and uses **`sync_fd[0/1] = -1`** (inactive). The all-not-found fast path (**`pip_all_nf()`** in **`msh_executor_pipeline_all_not_found.c`**) addresses stderr ordering for the “every stage not found” case without an active barrier. |
| **tokens** | Output of tokenizer; input to parser; freed after parse or on syntax error. |
| **cmds** | Output of parser; input to heredoc + executor; freed after execution or on error. |
| **input** | Current line from `readline` (TTY) or `ft_read_stdin_line` via **`shell_repl.c`** (non-TTY); freed in tokenizer or in `reset_shell()`. |
| **word_quoted** | Internal flag set by `mark_word_quoted()` during tokenization; tells `flush_word()` whether the current token came from a quoted span. |
| **hd_mod** | Set when `<<` is tokenized (`handle_operator`); when active, the tokenizer does not expand `$` in the heredoc delimiter string. |
| **oom** | Set to 1 by any tokenizer or parser helper when `malloc`/`ft_calloc` fails mid-parse; callers check this flag to propagate OOM without passing extra parameters. |

**Why one shell struct:** Single place for “current line’s” state (tokens, commands, input) and persistent state (env, cwd, last_exit). No global state except `g_signum` for signals.

---

### 1.7 Heredoc context: `t_heredoc_ctx`

```c
typedef struct s_heredoc_ctx {
    t_command  *cmd;        // command that owns this heredoc
    t_shell    *shell;      // shell reference (for expansion, signals)
    int         pipe_fd[2]; // pipe: write end for body, read end becomes cmd->hd_fd
    int         expand;     // 1 = expand $VAR in body; 0 = literal (quoted delimiter)
} t_heredoc_ctx;
```

| Field | Purpose |
|-------|--------|
| **cmd** | The command this heredoc belongs to; its `hd_fd` is set after reading. |
| **shell** | Needed for `exp_hd_line()` (variable values) and signal checking. |
| **pipe_fd** | The heredoc body is written to `pipe_fd[1]`; `pipe_fd[0]` becomes `cmd->hd_fd` for execution. |
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
    t_shell ||--o| t_list_tok : "tokens list"
    t_shell ||--o| t_list_cmd : "cmds list"
    t_shell ||--o| envp : "envp"
    t_list_tok ||--o| t_token : "content"
    t_list_cmd ||--o| t_command : "content"
    t_command ||--o| t_list_args : "args"
    t_command ||--o| t_list_redir : "redirs"
    t_list_args ||--o| t_arg : "content"
    t_list_redir ||--o| t_redir : "content"
    t_heredoc_ctx }o--|| t_command : "cmd"
    t_heredoc_ctx }o--|| t_shell : "shell"
```

**`t_list_tok`**, **`t_list_cmd`**, **`t_list_args`**, and **`t_list_redir`** are all **`t_list`** in code; **`argv`** is a derived `char **` field on **`t_command`** (not a separate ER entity).

---

## Part 2: Function Reference

Functions are grouped by **source file**. Each row: function name, return type / signature summary, and a short description.

### 2.1 Main, REPL & init

| File | Function | Description |
|------|----------|-------------|
| **main.c** (src/) | `main(argc, argv, envp)` | Zeros `t_shell`, `init_shell()`, `set_signals_interactive()`, sets `rl_event_hook` on TTY, **`shell_loop()`**, `rl_clear_history()`, `free_all()`, closes std fds, returns `shell.last_exit`. |
| **core/shell_repl.c** | `shell_loop(shell)` | REPL: `check_signal_received`, `read_input`; **`RL_EOF`** break; **`RL_SIG`** continue; read-path **`OOM`**: **`last_exit` FAILURE, `reset_shell`, break** (exit toward **`main`**); else **`repl_after_read`** (`process_input` if `input[0]`, syntax_err / `reset_shell`, non-TTY break on **`XSYN`**). |
| **core/shell_repl.c** | *(static)* `read_input(shell)` | TTY: `build_prompt` + `readline`; else **`read_line_stdin`** → **`ft_read_stdin_line(..., 0)`**. Returns **`RL_LN`**, **`RL_EOF`**, **`RL_SIG`**, or **`OOM`**. |
| **core/shell_repl.c** | *(static)* `build_prompt(shell)` | Allocates exact length, builds prompt with **`ft_strlcat`** (libft); caller frees after `readline`. |
| **core/shell_repl.c** | *(static)* `read_line_stdin(shell, out)` | Wrapper: `ft_read_stdin_line(shell, out, 0)`. |
| **core/shell_repl.c** | *(static)* `repl_after_read(shell)` | If `input[0]` → `process_input`; syntax_err when `!shell->cmds && last_exit == XSYN`; `reset_shell`; returns 1 to break on non-TTY + syntax_err. |
| **utils/msh_stdin_read_line.c** | `ft_read_stdin_line(shell, line, set_shell_oom_on_fail)` | Non-TTY: byte-read until `\n` or EOF; **`RL_LN`** / **`RL_EOF`** / **`OOM`**; optional `shell->oom` on append failure. |
| **core/init.c** | `process_input(shell)` | `tokenize_input` → `parse_input` → optional `process_heredocs` (on failure: SIGINT → `XSINT`, else `FAILURE`) → `run_commands` → assigns `last_exit`. |
| **core/init.c** | `init_shell(shell, envp)` | `init_shell_identity` → `init_runtime_fields`; interactive TTY → `update_shlvl`. |
| **core/init_utils.c** | `init_shell_identity(shell, envp)` | `ft_arrdup` envp, `USER`, `getcwd` (fallback `/`); on fatal alloc path `ft_dprintf` + `exit_norl(shell, FAILURE)`. |
| **core/init_utils.c** | `init_runtime_fields(shell)` | `ensure_default_envs`, `last_exit=SUCCESS`, `barrier_write_fd=-1`, null `tokens`/`cmds`/`input`, `word_quoted`/`hd_mod` 0, **`had_path`** from **`PATH`**. |
| **core/init_utils.c** | `get_env_value(envp, key)` | Returns pointer into `envp[i]` at value after `=`, or NULL. |

---

### 2.2 Utils & memory

| File | Function | Description |
|------|----------|-------------|
| **utils/msh_char_buffer_realloc.c** | `ft_realloc(ptr, new_size)` | Reallocates buffer; copies min(old_len, new_size-1); frees old. |
| **utils/msh_strarray_dup.c** | `ft_arrdup(envp)` | Duplicates `char**` array (for envp). |
| **utils/msh_string_expand_scan.c** | `msh_is_blank(c, ifs_mode)` | `ifs_mode` 0: space+tab (tokenizer); 1: space+tab+newline (default IFS; not full `ft_isspace`). |
| **utils/msh_string_expand_scan.c** | `msh_env_var_body_span`, `msh_is_dollar_var_leader`, … | Shared `$NAME` tail and heredoc `$` leader checks (uses `ft_isalnum` / `ft_isalpha`). |
| **free/free_exit.c** | `exit_norl(shell, status)` | `free_all`, close std fds, `exit` — init OOM before any `readline()` (no `rl_clear_history`). |
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
| **utils/msh_string_append_char.c** | `append_char(shell, dst, c)` | Appends one char to *dst via `ft_realloc`. |
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
| **tokenizer/expansion.c** | `exp_var(shell, i)` | Expands one variable at *i ($VAR or $?); advances *i; returns new string (caller frees). |
| **tokenizer/expansion_word.c** | `exp_dollar(shell, i, word)` | If input at *i is `$` and expandable, expands and appends to word. |
| **tokenizer/expansion_word.c** | `exp_tilde(shell, i, word)` | Expands `~` to HOME and appends to word. |
| **utils/msh_word_append_expanded.c** | `exp_q_cat(word, exp)` | Appends expanded segment `exp` to *word (quotes + heredoc growth). |
| **tokenizer/expansion_word.c** | `exp_unq(shell, word, exp, tokens)` | Appends expansion result; may split into multiple WORDs (blanks). |
| **tokenizer/expansion_utils.c** | `exp_empty(shell, start, end, word)` | Handles empty expansion in unquoted context (inserts `EMPTY_EXPAND` or adjusts for ambiguous redirect). |
| *(continuation support removed — unclosed quotes now emit a syntax error immediately)* |

---

### 2.4 Parser (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/parser.c** | `parse_input(shell)` | `syntax_check`; `build_command_list` + `finalize_cmds`; frees token list; **`OOM`** from finalize sets **`shell->oom`**, frees cmds. |
| **parser/parser_build.c** | `build_command_list(shell, tokens)` | Walks tokens into `t_list` of `t_command *`; **`PR_ERR`** → drop partial pipeline; **`OOM`** → **`shell->oom`**, drop pipeline. |
| **parser/parser_redir.c** | `parse_redir_token_pair(cmd, tok_node)` | Redir + WORD → `cmd->redirs`; **`PR_ERR`** if missing word; **`OOM`** if malloc fails. |
| **parser/add_token_to_cmd.c** | `add_token_to_command(...)` | WORD/HEREDOC/redir dispatch; **`PR_ERR`** structural; **`OOM`** on allocation failure. |
| **parser/argv_build.c** | `finalize_cmds(shell, cmd_list)` | Per command: `finalize_argv`; returns **`OOM`** on allocation failure (else **0**). |
| **parser/argv_build.c** | `finalize_argv(shell, cmd)` | Builds cmd->argv from cmd->args (NULL-terminated array). |
| **parser/parser_syntax_check.c** | `syntax_check(lst)` | Walks token `t_list`; validates pipes and redir+WORD. |
| **parser/parser_syntax_check.c** | `syntax_error(msg)` | Prints "minishell: syntax error near unexpected token 'msg'" to stderr; returns **FAILURE**. |

---

### 2.5 Heredoc (src/parser/)

| File | Function | Description |
|------|----------|-------------|
| **parser/parser.c** | `process_heredocs(shell)` | Walks commands; if `hd_delim`, `read_heredoc`; returns **FAILURE** on interrupt/error. |
| **parser/heredoc_input.c** | `heredoc_read_line(shell)` | TTY `readline("> ")` else `ft_read_stdin_line(..., 1)` (sets `shell->oom` on OOM). |
| **parser/heredoc_input.c** | `print_heredoc_eof_warning(line_no, delim)` | Bash-style EOF-before-delim warning to stderr. |
| **parser/heredoc_input.c** | `write_heredoc_line(line, fd, expand, shell)` | Writes one line to pipe write end; optional expansion. |
| **parser/heredoc.c** | `read_heredoc(cmd, shell, line_no)` | `pipe`, loop via **`heredoc_read_one`** / **`heredoc_read_line`**; sets `cmd->hd_fd`. |
| **parser/heredoc_utils.c** | `is_quoted_delimiter(delim)` | Returns 1 if delimiter is quoted (e.g. `'EOF'` or `"EOF"`) so body is not expanded. |
| **parser/heredoc_utils.c** | `exp_hd_line(line, shell)` | Expands `$VAR` and `$?` in line; returns new string (caller frees). |

---

### 2.6 Executor (src/executor/, `msh_executor_*` files; public API unprefixed)

| File | Function | Description |
|------|----------|-------------|
| **executor/msh_executor_run_commands.c** | `run_commands(shell)` | No commands → success; single cmd → `run_empty_command` / `run_single_builtin` / `run_external`; else `run_pip`. Returns last status. |
| **executor/msh_executor_run_commands.c** | *(static)* `run_empty_command` | Redirs/heredoc only: `backup_stdio_fds`, `apply_redirs`, `restore_stdio_fds`. |
| **executor/msh_executor_run_commands.c** | *(static)* `run_single_builtin` | Parent-only for **cd / export / unset / exit**; if another builtin has redirs/heredoc fd → `run_external`; else dup/apply/restore and `run_builtin`. |
| **executor/msh_executor_run_commands.c** | *(static)* `backup_stdio_fds` / `restore_stdio_fds` | Dup stdin/stdout for builtin redir in parent. |
| **executor/msh_executor_apply_redirects.c** | `apply_redirs(cmd)` | Walk `redirs` left-to-right (`apply_one_redir`); then if `hd_fd` ≥ 0, dup to stdin only when **`stdin_last == STDIN_LAST_HD`**, and always close the heredoc read fd. |
| **executor/msh_executor_external_run.c** | `run_external(cmd, shell)` | Fork; child `set_signals_default`, `apply_redirs`, `run_in_child`; parent `set_signals_ignore`, `waitpid`, `set_signals_interactive`, `child_wait_st`. |
| **executor/msh_executor_external_run.c** | *(static)* `child_wait_st` | `WIFEXITED` → `WEXITSTATUS`; `WIFSIGNALED` → `XSB + WTERMSIG`; SIGQUIT prints “Quit (core dumped)”. |
| **executor/msh_executor_external_run.c** | *(static)* `scan_path` / `path_cand` | Colon-scan PATH with `stat` (regular file). |
| **executor/msh_executor_external_run.c** | `resolve_cmd_path(cmd, shell)` | **`PATH_MAX`** static buffer; absolute or PATH search; default list if `had_path` and PATH unset. |
| **executor/msh_executor_child_run.c** | *(static)* `run_builtin_in_child` | SIGPIPE ignore, `exit_norl(shell, run_builtin(...))`. |
| **executor/msh_executor_child_run.c** | *(static)* `child_exit_not_found` / `child_abort_msg` | Error messages + `exit_norl` with `XNF` / `XNX`. |
| **executor/msh_executor_child_run.c** | `run_in_child(cmd, shell)` | Builtin branch, empty argv, `resolve_cmd_path`, `execve` or errors. |
| **executor/msh_executor_pipeline_all_not_found.c** | *(static)* `is_simple_not_found_command` | Predicate: simple missing-PATH external, no redir/heredoc, no `/` in argv0. |
| **executor/msh_executor_pipeline_all_not_found.c** | `pip_all_nf(cmds, shell)` | If every stage matches **`is_simple_not_found_command`**, print all “not found” via **`put_cmd_not_found`** in parent; returns **TRUE** so **`run_pip`** returns **`XNF`**. |
| **executor/msh_executor_not_found_stderr.c** | `put_cmd_not_found` (+ static `format_not_found_name`, `needs_dollar_quotes`, `append_escaped_char`, `fill_dollar_quoted_name`) | One stderr line for not-found (`$'…'` when name has control bytes). |
| **executor/msh_executor_pipeline.c** | *(static)* `wait_pipes` | `waitpid(-1,…)` until **n** children; status of **last_pid** wins (`upd_wait_st`). |
| **executor/msh_executor_pipeline.c** | *(static)* `spawn_pipes` | Chains `pipe_step`, closes trailing `prev_fd`. |
| **executor/msh_executor_pipeline.c** | `run_pip(cmds, shell)` | `sync_fd` inactive (-1); `pip_all_nf` → **`XNF`**; ignore signals, loop, wait, restore interactive. |
| **executor/msh_executor_pipeline_segment.c** | *(static)* `setup_pip_child_fds` / `fork_pip_child` / `advance_prev_pipe_fd` | Pipe wiring between stages. |
| **executor/msh_executor_pipeline_segment.c** | `pipe_step(cmd, shell, prev_fd, sync_fd)` | One pipeline segment; used by `msh_executor_pipeline.c`. |

---

### 2.7 Builtins & dispatcher (src/builtins/)

| File | Function | Description |
|------|----------|-------------|
| **builtins/builtin_dispatcher.c** | `get_builtin_type(cmd)` | Returns enum (`B_ECHO`, etc.) or `B_NONE`; `argv_build.c` uses `!= B_NONE` to set `cmd->is_builtin`. |
| **builtins/builtin_dispatcher.c** | `run_builtin(argv, shell)` | Looks up `argv[0]` and calls the matching `builtin_*` via the static registry inside `builtin_registry()`. |
| **builtins/echo.c** | `builtin_echo(args, shell)` | Prints args to stdout with spaces; handles -n (no newline). Returns 0. |
| **builtins/cd.c** | `builtin_cd(args, shell)` | **`cd`**, **`cd --`**, **`cd -- /path`**, **`-`**, **`$HOME`**; two operands → too many args; updates PWD/OLDPWD and **`shell->cwd`**. |
| **builtins/pwd.c** | `builtin_pwd(args, shell)` | Prints current working directory. Returns 0. |
| **builtins/env.c** | `builtin_env(args, shell)` | Prints envp (one KEY=value per line). Returns 0. |
| **builtins/export.c** | `builtin_export(args, shell)` | No args: print declare -x list; with args: add/update env; invalid name → error, return 1. |
| **builtins/export_utils.c** | `export_invalid_identifier_err(arg)` | Prints one `export: \`arg': not a valid identifier` line to stderr. |
| **builtins/export_utils.c** | `is_valid_export_name(name)` | Returns 1 if name is valid for export (letter/underscore start, alnum/_). |
| **builtins/export_utils.c** | `find_export_key_index(shell, key, key_len)` | Finds index of `KEY=value` or bare `KEY` in envp. |
| **builtins/export_utils.c** | `append_export_env(shell, entry)` | Appends one "KEY=value" to shell->envp. |
| **builtins/export_print.c** | `print_sorted_env(shell)` | Prints env in declare -x format (sorted). |
| **builtins/unset.c** | `builtin_unset(args, shell)` | Removes listed vars from envp. Returns 0/1. |
| **builtins/exit.c** | `builtin_exit(args, shell)` | Exits shell: optional numeric status; too many args → **`FAILURE`**; non-numeric → **`XSYN`** (bash: 255). Calls `clean_exit` / `free_all`. |
| **builtins/exit_utils.c** | `parse_exit_value(str, result)` | Parses a `long long` exit value with overflow checks; skips leading/trailing bytes where `ft_isspace` is true. Returns 1 on success, 0 on error. |

---

### 2.8 Signals & free

| File | Function | Description |
|------|----------|-------------|
| **signals/signal_handler.c** | `set_signals_default()` | SIGINT/SIGQUIT/SIGPIPE/SIGTERM → **SIG_DFL** (after fork, before exec). |
| **signals/signal_handler.c** | `set_signals_ignore()` | SIGINT/SIGQUIT → **SIG_IGN** (parent while waiting on child/pipeline). |
| **signals/signal_handler.c** | `set_signals_interactive()` | SIGQUIT/SIGTERM/SIGPIPE ignored; SIGINT → **`interactive_sigint_handler`** with **SA_RESTART** and mask **SIGQUIT**; clears **`g_signum`**. |
| **signals/signal_utils.c** | `readline_event_hook()` | If **`g_signum == SIGINT`**: clear readline line, **`rl_done = 1`**. |
| **signals/signal_utils.c** | `check_signal_received(shell)` | If **`g_signum == SIGINT`**: **`last_exit = XSINT`**, clear flag, return 1. |
| **free/free_utils.c** | `free_tokens(&lst)` | `ft_lstclear` on token list; each `content` is `t_token *`. |
| **free/free_utils.c** | `free_args(&lst)` | `ft_lstclear` on arg list; each `content` is `t_arg *`. |
| **free/free_runtime.c** | `free_cmds(&lst)` | `ft_lstclear` on command list; each payload is one `t_command` (args/redirs as lists). |
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
    ri --> gate{"input[0] non-zero?"}
    gate -->|yes| proc[process_input]
    gate -->|no| rs[reset_shell]
    proc --> tok[tokenize_input]
    tok --> par[parse_input]
    par --> hd[process_heredocs]
    hd --> ex[run_commands]
    ex --> br{single vs pipeline}
    br --> emp[run_empty_command / run_single_builtin / run_external]
    br --> pl[run_pip → pipe_step]
    emp --> ich[run_in_child / exit_norl in child]
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
| [`includes/defines.h`](../includes/defines.h) | **`SUCCESS` / `FAILURE`**, **`TOK_N` / `TOK_Y`**, **`RL_LN` / `RL_EOF` / `RL_SIG`**, **`XSYN`**, **`XNF` / `XNX`**, **`XSB`**, **`XSINT`**, **`OOM`**, **`PR_ERR`**, **`STDIN_LAST_*`**, **`HD_*`**, **`S_EMPTY` / `S_AMBIG`**, **`PM_*`**, etc. |
| [SIGNAL.md](SIGNAL.md) | Interactive vs wait vs child signal handling; **`g_signum`**. |

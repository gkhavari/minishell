# Minishell Project Architecture (Defensive Programming)

> **Philosophy:** Defensive programming means we validate every input, handle every error case explicitly, and never assume success. We use bash as our reference implementation but only implement what the 42 subject requires.
>
> **Test-backed behavior:** Primary harness is **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** (`tester.sh`, mandatory mode `m`). For expected I/O and exit codes, see **[BEHAVIOR.md](BEHAVIOR.md)**.

---

## 0. Project Status & Built Implementation

This section reflects the **actual codebase** as built: source layout, data flow, and test status.

### 0.1 Test harness (current repo)

| Where | What runs | Notes |
|-------|-----------|--------|
| **Local (Docker)** | `./scripts/run_minishell_tester.sh [mode]` | Clones [LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester) into the dev container (`/root/42_minishell_tester`), builds `/app`, then `tester.sh --no-update <mode>`. Default mode `m` = mandatory. Common: `m`, `vm`, `b`, `ne`, `a`, `va` (see script header). |
| **GitHub Actions** | `.github/workflows/test.yaml` + `regression_test.yaml` | Same upstream: `git clone https://github.com/LeaYeh/42_minishell_tester.git`. Regression matrix: `m` (mandatory), `b` (bonus), `ne` (empty env). Crash: `a` / `a --no-env`. Valgrind: one job per `cmds/**/*.sh` with `tester.sh va`. |
| **Optional wrapper** | `make -C tests test` | Documented in [README.md](../README.md) if a `tests/` Makefile is present; not required if you only use the Docker script. |

```mermaid
flowchart LR
    subgraph local["Local dev"]
        A[./scripts/run_minishell_tester.sh] --> B[docker exec 42-Docker-DevEnv]
        B --> C[LeaYeh tester.sh]
    end
    subgraph ci["GitHub Actions"]
        D[checkout + make re] --> E[clone LeaYeh 42_minishell_tester]
        E --> F[tester.sh m / b / ne / a / va]
    end
```

Pass/fail is **not** pinned here (depends on branch and last CI run). Use workflow logs or `mstest_output_*` from the tester for results.

### 0.2 Test coverage map (LeaYeh `cmds/mand/`)

Script names below match **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** (mandatory part). Behavior detail stays in [BEHAVIOR.md](BEHAVIOR.md).

| Area | Scripts (indicative) |
|------|----------------------|
| **Echo** | `1_builtins_echo.sh` |
| **pwd** | `1_builtins_pwd.sh` |
| **cd** | `1_builtins_cd.sh` |
| **env** | `1_builtins_env.sh` |
| **export** | `1_builtins_export.sh` |
| **unset** | `1_builtins_unset.sh` |
| **exit** | `1_builtins_exit.sh` |
| **Variables / expansion** | `1_variables.sh`, `11_expansion.sh`, plus parsing/compare scripts |
| **Redirections** | `1_redirs.sh` |
| **Pipes** | `1_pipelines.sh` |
| **Syntax** | `8_syntax_errors.sh` |
| **Path / 127 / 126 / stress** | `1_scmds.sh`, `2_path_check.sh`, `9_go_wild.sh` |
| **Parsing / compare** | `0_compare_parsing.sh`, `10_parsing_hell.sh` |
| **Misc correction-style** | `2_correction.sh` |

### 0.3 Source Layout (real files)

**Rule:** Only `main.c` lives in `src/` root. All other sources are in subfolders.

| Directory | Purpose |
|-----------|---------|
| `src/` | `main.c` only — REPL loop, read_input, process_input |
| `src/core/` | `init.c` (`init_shell`, `process_input`), `init_runtime.c` (`init_runtime_fields`), `init_utils.c` (`get_env_value`, `build_prompt`, `move_last_env_to_front`) |
| `src/utils/` | General utilities: `utils.c` (ft_arrdup, msh_calloc, ft_strcat, ft_realloc) |
| `src/free/` | Memory cleanup: `free_utils.c`, `free_runtime.c`, `free_shell.c` |
| `src/signals/` | Signal handlers and readline hook |
| `src/tokenizer/` | Lexer: tokenizer.c, expansion, quote/operator handlers, utils |
| `src/parser/` | Parser: parser.c, syntax_check, argv_build, heredoc, heredoc_utils, heredoc_warning |
| `src/executor/` | `executor.c`, `executor_utils.c`, `executor_external.c`, `executor_pipeline.c`, `executor_pipeline_steps.c`, `executor_pipeline_not_found.c`, `executor_child_exec.c`, `executor_child_format.c` |
| `src/builtins/` | Builtin commands and dispatcher, export_print, exit_utils |

```mermaid
graph TB
    subgraph Entry
        main[main.c]
    end
    subgraph core
        init[core/init.c]
        init_rt[core/init_runtime.c]
        init_u[core/init_utils.c]
    end
    subgraph utils
        utils_file[utils/utils.c]
    end
    subgraph free
        free_utils[free/free_utils.c]
        free_runtime[free/free_runtime.c]
        free_shell[free/free_shell.c]
    end
    subgraph Signals
        sig_handler[signals/signal_handler.c]
        sig_utils[signals/signal_utils.c]
    end
    subgraph Tokenizer
        tok[tokenizer.c]
        tok_utils[tokenizer_utils.c]
        tok_utils2[tokenizer_utils2.c]
        tok_ops[tokenizer_ops.c]
        tok_handlers[tokenizer_handlers.c]
        tok_quotes[tokenizer_quotes.c]
        expansion[expansion.c]
        expansion_utils[expansion_utils.c]
    end
    subgraph Parser
        parser[parser.c]
        syntax[parser_syntax_check.c]
        add_token[add_token_to_cmd.c]
        argv_build[argv_build.c]
        heredoc[heredoc.c]
        heredoc_utils[heredoc_utils.c]
        heredoc_warn[heredoc_warning.c]
    end
    subgraph Executor
        exec[executor.c]
        exec_utils[executor_utils.c]
        exec_external[executor_external.c]
        exec_pipeline[executor_pipeline.c]
        exec_pipe_steps[executor_pipeline_steps.c]
        exec_child_exec[executor_child_exec.c]
        exec_child_fmt[executor_child_format.c]
        exec_pipe_nf[executor_pipeline_not_found.c]
    end
    subgraph Builtins
        dispatcher[builtin_dispatcher.c]
        echo[echo.c]
        cd[cd.c]
        pwd[pwd.c]
        env[env.c]
        export[export.c]
        export_utils[export_utils.c]
        export_print[export_print.c]
        unset[unset.c]
        exit[exit.c]
        exit_utils[exit_utils.c]
    end
    main --> init
    init --> init_rt
    init --> init_u
    main --> utils_file
    main --> tok
    main --> parser
    main --> exec
    tok --> parser
    parser --> exec
    exec --> dispatcher
    exec --> exec_utils
    exec --> exec_external
    exec --> exec_pipeline
    exec_pipeline --> exec_pipe_steps
    exec_pipeline --> exec_pipe_nf
    exec_external --> exec_child_exec
    exec_pipe_steps --> exec_child_exec
    exec_child_exec --> exec_child_fmt
    dispatcher --> echo
    dispatcher --> cd
    dispatcher --> pwd
    dispatcher --> env
    dispatcher --> export
    dispatcher --> unset
    dispatcher --> exit
    exit --> exit_utils
```

### 0.4 Pipeline: Input → Execution (real flow)

```mermaid
flowchart LR
    A[readline TTY / read_line_stdin non-TTY] --> B[tokenize_input]
    B --> C[parse_input]
    C --> D[process_heredocs]
    D --> E[execute_commands]
    E --> F[reset_shell]
    B --> G[shell->tokens]
    C --> H[shell->commands]
    E --> I[run_empty / run_builtin_command / execute_external / execute_pipeline]
    I --> J[run_builtin OR execute_in_child in child]
```

- **main.c** (src/): `shell_loop` → `read_input` → `process_input` (tokenize → parse → heredocs → execute) → `reset_shell`.
- **Tokenizer** (src/tokenizer/): `tokenize_input()` in `tokenizer.c`; uses `tokenizer_handlers.c`, `tokenizer_quotes.c`, `expansion.c`, and `tokenizer_ops.c`.
- **Parser** (src/parser/): `parse_input()` in `parser.c`; `syntax_check()` in `parser_syntax_check.c`; `finalize_all_commands()` in `argv_build.c` builds `argv` and sets `is_builtin`.
- **Executor** (src/executor/): `execute_commands()` in `executor.c` — empty argv, `run_empty_command`; else single command: `run_builtin_command` (parent with optional `dup`/`apply_redirections`/`restore_fds`, or `execute_external` if builtin has redirs and is not `must_run_in_parent`) or `execute_external`; pipeline → `execute_pipeline()` → `run_pipe_step` / `wait_children_last`; child path → `execute_in_child()` in `executor_child_exec.c`.

---

## 1. Global State & Signal Handling

### 1.1 The Global Variable Rule

```c
/* ONLY global variable allowed in the entire project */
volatile sig_atomic_t	g_signum = 0;
```

| Value        | Meaning         | Action Required                                    |
| ------------ | --------------- | -------------------------------------------------- |
| `0`          | No signal       | Continue normally                                  |
| `SIGINT (2)` | Ctrl+C received | Print newline, new prompt, set `last_exit = 130` |

**Critical Rules:**

- ❌ Do NOT store structs, pointers, or flags in global
- ❌ Do NOT access `t_shell` from signal handler
- ✅ Only store the signal NUMBER
- ✅ Check `g_signum` in main loop AFTER `readline()` returns

**Why `volatile sig_atomic_t`?**

- `volatile`: Tells compiler the value can change at any time (by signal handler)
- `sig_atomic_t`: Guaranteed atomic read/write (no partial updates)

### 1.2 Signal Behavior (Bash Reference)

| Signal             | Interactive Mode     | During Execution                  | During Heredoc                     |
| ------------------ | -------------------- | --------------------------------- | ---------------------------------- |
| `SIGINT` (Ctrl+C)  | New prompt, `$?=130` | Kill child, new prompt            | Stop heredoc, new prompt, `$?=130` |
| `SIGQUIT` (Ctrl+\) | Ignored              | Kill child + "Quit (core dumped)" | Ignored                            |
| `EOF` (Ctrl+D)     | Exit shell           | N/A (not a signal)                | Close heredoc input                |

**Implementation Pattern:**

```c
/* In main loop, AFTER readline returns */
if (g_signum == SIGINT)
{
    shell->last_exit = 130;
    g_signum = 0;  /* Reset for next iteration */
}
```

### 1.3 Initialization (actual implementation)

**Current `init_shell()` behavior** (see `src/core/init.c`, `init_runtime.c`):

```
init_shell(t_shell *shell, char **envp)
├── 1. Caller zeros struct first: ft_bzero(shell) in main
├── 2. shell->envp = ft_arrdup(envp); clean_exit if NULL
├── 3. shell->user: USER in env → msh_strdup; else NULL
├── 4. shell->cwd = getcwd(NULL, 0); if NULL → msh_strdup("/")
├── 5. init_runtime_fields(shell) — ensure_default_envs (PWD, SHLVL, _), last_exit=0,
│      tokens/commands/input NULL, word_quoted/heredoc_mode 0, barrier_write_fd=-1, had_path
└── 6. if isatty(STDIN): update_shlvl(shell) — bump SHLVL in envp
Signal handlers: main() calls set_signals_interactive() after init_shell
```

**Shell struct fields** (see `includes/structs.h`): In addition to the core fields above, `t_shell` includes three internal flags:
- **`barrier_write_fd`**: Write FD for the pipeline launch barrier (or -1 if not active). Used to synchronize child startup in pipelines to reduce stderr ordering variance.
- **`word_quoted`**: Internal tokenizer flag — marks the current word as containing quoted content.
- **`heredoc_mode`**: Internal tokenizer flag — suppresses `$` expansion inside heredoc delimiter parsing.

See [BEHAVIOR.md](BEHAVIOR.md) for runtime semantics. For **42 tester–related fixes** (token list, USER, PATH, export/unset, etc.), see [42_tester_failures.md](42_tester_failures.md).

---

## 2. Main Loop (REPL Cycle)

**Implementation:** `main.c` → `shell_loop()` → `read_input()` → `process_input()`.

**Input source:** When stdin is a TTY, `read_input()` uses `readline(prompt)` (history, editing). When stdin is not a TTY (e.g. pipe from the 42_minishell_tester), it uses **`read_line_stdin()`** in `main.c`: byte-by-byte `read()` until `\n` or EOF—no prompt, no readline. One call returns one logical line (without the newline).

```mermaid
flowchart TD
    START([shell_loop]) --> CHECK_SIG[check g_signum]
    CHECK_SIG --> READ[read_input]
    READ --> PROMPT{isatty?}
    PROMPT -->|yes| BUILD[build_prompt, readline]
    PROMPT -->|no| RLS[read_line_stdin]
    BUILD --> GOT{input}
    RLS --> GOT{input}
    GOT -->|NULL| EXIT[print exit, break]
    GOT -->|empty| RESET[reset_shell, continue]
    GOT -->|string| CHECK_SIG2[check g_signum again]
    CHECK_SIG2 --> PROCESS[process_input]
    PROCESS --> TOKEN[tokenize_input]
    TOKEN --> PARSE[parse_input]
    PARSE --> HEREDOC[process_heredocs]
    HEREDOC --> EXEC[execute_commands]
    EXEC --> RESET
    RESET --> CHECK_SIG
```

| Step | Code / behavior |
|------|------------------|
| 1 | `check_signal_received(shell)` — if SIGINT, set `last_exit=130`, reset `g_signum`. |
| 2 | `read_input()`: if TTY → `build_prompt()`, `readline(prompt)`; else `read_line_stdin()` (one line per call, no prompt). |
| 3 | NULL → print `"exit"` (TTY only), break; empty → skip processing; else continue. |
| 4 | Check `g_signum` again (e.g. Ctrl+C during readline). |
| 5 | **TTY:** non-empty input is added to readline history in `tokenizer_handlers.c` during tokenization (before `shell->input` is freed). **Non-TTY:** no history. |
| 6 | `process_input()`: tokenize → parse → heredocs → execute. |
| 7 | `reset_shell(shell)` frees tokens, commands, input. |

---

## 3. Lexer (Tokenization)

### 3.1 Token Types (Matching Your structs.h)

```c
typedef enum e_tokentype
{
    WORD,      /* Commands, arguments, filenames */
    PIPE,      /* | */
    REDIR_IN,  /* < */
    REDIR_OUT, /* > and >| (clobber treated as plain redirect) */
    APPEND,    /* >> */
    HEREDOC,   /* << */
}   t_tokentype;
```

> **Note:** `2>` (stderr redirect) is **not implemented** — not a mandatory requirement. The tokenizer treats `2` as a WORD and `>` as `REDIR_OUT`.

### 3.2 Lexer State Machine

```
Input: echo "hello world" | cat < file.txt

State: NORMAL
        │
        ├── Whitespace → Skip
        ├── Quote (' or ") → Enter QUOTED state
        ├── | → Emit PIPE token
        ├── < → Check next char
        │       ├── < → Emit HEREDOC
        │       └── else → Emit REDIR_IN
        ├── > → Check next char
        │       ├── > → Emit APPEND
        │       ├── | → Emit REDIR_OUT (clobber >| treated as plain >)
        │       └── else → Emit REDIR_OUT
        └── Other → Accumulate into WORD  (note: "2>" → WORD "2" + REDIR_OUT)

State: SINGLE_QUOTED (')
        └── Everything is literal until closing '

State: DOUBLE_QUOTED (")
        ├── $ → Mark for expansion (but still in WORD)
        └── Everything else literal until closing "
```

### 3.3 Syntax Error Detection (Defensive Checks)

**Error: Unclosed Quotes**

```bash
$ echo "hello        # bash: unexpected EOF while looking for matching `"'
$ echo 'hello        # bash: unexpected EOF while looking for matching `''
```

**Our behavior:** Print error, set `last_exit = 2`, do NOT execute.

**Error: Invalid Pipe Usage**

```bash
$ | ls              # bash: syntax error near unexpected token `|'
$ ls |              # bash: syntax error near unexpected token `newline'
$ ls || cat         # We don't handle || (logical OR) - treat as syntax error
$ ls | | cat        # bash: syntax error near unexpected token `|'
```

**Our behavior:** Print `minishell: syntax error near unexpected token`, set `last_exit = 2`.

**Error: Invalid Redirection**

```bash
$ ls >              # bash: syntax error near unexpected token `newline'
$ ls > > file       # bash: syntax error near unexpected token `>'
$ ls < >            # bash: syntax error near unexpected token `>'
```

### 3.4 Syntax Validation (actual: `parser_syntax_check.c`)

```c
/* Leading PIPE → error. Then: PIPE cannot be last or doubled; each redir needs WORD next. */
int	syntax_check(t_token *token)
{
	if (!token)
		return (SYNTAX_OK);
	if (token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE
			&& (!token->next || token->next->type == PIPE))
			return (syntax_error("|"));
		if (is_redirection(token->type))
		{
			if (!token->next)
				return (syntax_error("newline"));
			if (token->next->type != WORD)
				return (syntax_error(get_token_str(token->next->type)));
		}
		token = token->next;
	}
	return (SYNTAX_OK);
}
```

### 3.5 Verified by tests (lexer + syntax)

The following behaviors are verified by **Hardening** (no crash + correct exit / message):

| Input | Expected | Test name (hardening) |
|-------|----------|------------------------|
| `"` (empty) | No crash, exit 0 | empty string |
| `   ` (spaces) | No crash, exit 0 | spaces only |
| `|` | Syntax error, exit 2 | lone pipe |
| `||` | Syntax error, exit 2 | double pipe |
| `echo hi |` (pipe last) | Syntax error, exit 2 | pipe at end |
| `echo "hello` (unclosed) | No crash | unclosed double quote |
| `echo hi >` | Syntax error, exit 2 | redir no file, syntax redir no file |
| `>` | Syntax error, exit 2 | only redir token |
| pipe first (e.g. `| echo hi`) | stderr contains "syntax" | syntax pipe first |
| pipe last (e.g. `echo hi |`) | stderr contains "syntax" | syntax pipe last |

See [BEHAVIOR.md](BEHAVIOR.md) §1 for the full input-resilience table.

---

## 4. Expansion (Variable Substitution)

### 4.1 Expansion Order (Critical!)

```
┌──────────────────────────────────────────────────────────────┐
│  STEP 1: Variable Expansion ($VAR, $?)                       │
│  ─────────────────────────────────────────────────────────── │
│  • Happens INSIDE double quotes: "$HOME" → "/home/user"      │
│  • Does NOT happen inside single quotes: '$HOME' → "$HOME"   │
│  • Unset variable → empty string: $UNDEFINED → ""            │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  STEP 2: Quote Removal                                       │
│  ─────────────────────────────────────────────────────────── │
│  • "hello" → hello                                           │
│  • 'world' → world                                           │
│  • "hello"'world' → helloworld (concatenation)               │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  STEP 3: Word splitting (unquoted expansion only)           │
│  ─────────────────────────────────────────────────────────── │
│  • `append_expansion_unquoted()` splits on spaces/tabs        │
│  • Quoted segments use `append_expansion_quoted()` (no split) │
│  • Bash IFS is not implemented; split is whitespace-only    │
└──────────────────────────────────────────────────────────────┘
```

### 4.2 Variable Expansion Rules

| Input        | Context         | Result             | Explanation                |
| ------------ | --------------- | ------------------ | -------------------------- |
| `$HOME`      | Unquoted        | `/home/user`       | Normal expansion           |
| `"$HOME"`    | Double quotes   | `/home/user`       | Expansion works in ""      |
| `'$HOME'`    | Single quotes   | `$HOME`            | NO expansion in ''         |
| `$?`         | Any (except '') | `0` (or last exit) | Exit status                |
| `$UNDEFINED` | Any             | `` (empty)         | Unset → empty string       |
| `$`          | End of word     | `$`                | Literal $ (no var name)    |
| `$123`       | Any             | `$123`             | Invalid var name → literal |
| `"$"`        | Double quotes   | `$`                | Lone $ is literal          |

### 4.3 Variable Name Rules

```c
/* Valid variable name: starts with letter or _, followed by alnum or _ */
int is_valid_var_char(char c, int is_first)
{
    if (is_first)
        return (ft_isalpha(c) || c == '_');
    return (ft_isalnum(c) || c == '_');
}
```

### 4.4 Defensive Expansion Examples

```bash
# Test cases to verify your expander:

echo $HOME              # /home/user
echo "$HOME"            # /home/user
echo '$HOME'            # $HOME
echo $?                 # 0 (or last exit code)
echo "$?"               # 0
echo '$?'               # $?
echo $UNDEFINED         # (empty line)
echo "$UNDEFINED"       # (empty line)
echo $USER$HOME         # userhome (concatenated)
echo "$USER$HOME"       # user/home/user
echo $                  # $
echo "hello$"           # hello$
echo $123               # $123 (invalid var name)
echo $USER_NAME         # (value of USER_NAME, not USER + _NAME)
```

### 4.5 Verified by tests (expansion)

| Input / scenario | Expected | Test (phase1 / hardening) |
|------------------|----------|----------------------------|
| `echo $UNDEFINED` | Empty line | undefined var empty |
| `echo $` | `$` | dollar alone |
| `true` then `echo $?` | `0` | dollar question (success) |
| `false` then `echo $?` | `1` | dollar question (failure) |
| `echo $1` | Literal `$1` (no expand) | dollar digit no expand |
| `echo '$HOME'` | `$HOME` | var in single quotes |
| `export VAR=val` then `echo $VAR` | `val` | set and echo var (phase1 + hardening) |
| `echo "hello $VAR"` (VAR=world) | `hello world` | var in double quotes |
| `export X=xyz` then `unset X` then `echo $X` | Empty | unset var |
| `export A_B=1` then `echo $A_B` | `1` | var with underscore |
| `echo a$EMPTY b` | `a b` | empty var |
| `export A-B=x` | stderr "not a valid identifier" | invalid export |
| `echo $` at end of line | No crash | expansion at end |

Expansion runs during **tokenization** (see `tokenizer/expansion.c`, `tokenizer/expansion_utils.c`). Heredoc expansion is in `parser/heredoc_utils.c` (quoted delimiter → no expand). See [BEHAVIOR.md](BEHAVIOR.md) §4.

---

## 5. Parser (Command Table Construction)

### 5.1 Command Structure (actual: `includes/structs.h`)

```c
typedef struct s_arg
{
    char            *value;
    struct s_arg    *next;
}   t_arg;

typedef struct s_redir
{
    char            *file;
    int             fd;         /* Target stream: STDIN_FILENO, STDOUT_FILENO, or STDERR_FILENO */
    int             append;     /* 1 for >>; 0 for <, >, 2> */
    struct s_redir  *next;
}   t_redir;

typedef struct s_command
{
    t_arg               *args;          /* Linked list of args; finalize_argv → argv */
    char                **argv;         /* ["ls", "-la", NULL] for execve */
    t_redir             *redirs;        /* All redirections: < > >> << 2> (file, fd, append) */
    int                 heredoc_fd;     /* FD for heredoc input (or -1) */
    char                *heredoc_delim; /* Delimiter for heredoc */
    int                 heredoc_quoted; /* Flag if delimiter was quoted */
    int                 is_builtin;     /* Set in finalize_all_commands via is_builtin(argv[0]) */
    struct s_command    *next;          /* Next command in pipeline */
}   t_command;
```

- **Parser** fills `args` and `redirs`; **argv_build.c** `finalize_argv()` builds `argv`, then `finalize_all_commands()` sets `is_builtin`.

### 5.2 Parsing Flow (actual: `parser/parser.c`, `parser/add_token_to_cmd.c`)

```mermaid
flowchart LR
    T[Tokens]
    T --> P[parse_tokens]
    P --> C1[cmd1: args]
    P --> C2[cmd2: args + redirs]
    C1 --> F[finalize_all_commands]
    C2 --> F
    F --> A1[argv + is_builtin]
    F --> A2[argv + redirs]
```

- **parser/parser.c**: `parse_tokens()` walks tokens; each step `parse_token_step()` — on `PIPE` appends a new `t_command`, else `add_token_to_command()` (WORD → `add_word_to_cmd`, redirs → `append_redir` / `handle_heredoc_token`).
- **parser/argv_build.c**: `finalize_all_commands()` → `finalize_argv()` (args list → `argv[]`), then `is_builtin(cmd->argv[0])` → `cmd->is_builtin`.

### 5.3 Redirection Parsing (Right-to-Left for Multiple)

```bash
# Bash behavior: Last redirection wins
$ echo hello > file1 > file2    # Creates both, writes to file2
$ cat < file1 < file2           # Opens both, reads from file2

# Our approach (simpler): Process left-to-right, last one wins
# This matches bash behavior for the final result
```

### 5.4 Builtin Detection

```c
typedef enum e_builtin
{
    NOT_BUILTIN = 0,
    BUILTIN_ECHO,
    BUILTIN_CD,
    BUILTIN_PWD,
    BUILTIN_EXPORT,
    BUILTIN_UNSET,
    BUILTIN_ENV,
    BUILTIN_EXIT
}   t_builtin;

t_builtin   get_builtin_type(char *cmd)
{
    if (!cmd)
        return (NOT_BUILTIN);
    if (ft_strcmp(cmd, "echo") == 0)
        return (BUILTIN_ECHO);
    if (ft_strcmp(cmd, "cd") == 0)
        return (BUILTIN_CD);
    /* ... etc ... */
    return (NOT_BUILTIN);
}
```

---

## 6. Heredoc Handling

### 6.1 When to Process Heredocs

```
CRITICAL: Process ALL heredocs BEFORE forking for execution!

Why?
1. Heredoc reads from stdin (same as your prompt)
2. If you fork first, child and parent fight for stdin
3. Signals during heredoc need special handling
```

### 6.2 Heredoc Flow

```
Command: cat << EOF << END
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  1. Find all HEREDOC tokens in command list                  │
└──────────────────────────────────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  2. For each heredoc (left to right):                        │
│     a. pipe(); write lines to write end                      │
│     b. Read lines until delimiter                            │
│     c. Store read end in cmd->heredoc_fd                     │
└──────────────────────────────────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  3. Last heredoc FD becomes stdin for command                │
└──────────────────────────────────────────────────────────────┘
```

### 6.3 Heredoc + Signals

**Implementation:** `parser/heredoc.c` — static `read_heredoc_line()` uses `readline("> ")` on a TTY, else the same byte-by-byte pattern as `main.c` `read_line_stdin`. Loop checks `g_signum == SIGINT` after each line → close pipe ends, return `FAILURE` (`process_input` sets `last_exit = 130`). EOF without delimiter → `print_heredoc_eof_warning()` (see `heredoc_warning.c`).

### 6.4 Heredoc Expansion Rules

```bash
# Delimiter WITHOUT quotes: Expansion happens
cat << EOF
$HOME
EOF
# Output: /home/user

# Delimiter WITH quotes: No expansion (literal)
cat << 'EOF'
$HOME
EOF
# Output: $HOME

cat << "EOF"
$HOME
EOF
# Output: $HOME (same as single quotes for delimiter)
```

---

## 7. Executor (The Core Engine)

**Implementation:** `executor/executor.c` (`execute_commands`, static `run_empty_command` / `run_builtin_command`), `executor/executor_utils.c` (`apply_redirections`), `executor/executor_external.c` (`execute_external`, `find_command_path`), `executor/executor_pipeline.c` + `executor_pipeline_steps.c` (`run_pipe_step`, barrier-ready child setup), `executor/executor_pipeline_not_found.c`, `executor/executor_child_exec.c` (`execute_in_child`), `executor/executor_child_format.c` (`write_err3`, `format_cmd_name_for_error`).

### 7.1 Decision Tree (real code path)

```mermaid
flowchart TD
    EC[execute_commands]
    EC --> NO_CMD{shell->commands?}
    NO_CMD -->|NULL| R0[return SUCCESS]
    NO_CMD -->|non-NULL| ONE{commands->next?}
    ONE -->|NULL| EMP{argv missing or argv0 empty?}
    EMP -->|yes| EMPTY[run_empty_command redir only]
    EMP -->|no| BUILTIN{cmd->is_builtin?}
    BUILTIN -->|yes| RB[run_builtin_command]
    BUILTIN -->|no| EXT[execute_external]
    RB --> RBX{redirs and not must_run_in_parent?}
    RBX -->|yes| EXT
    RBX -->|no| RBP[dup2/apply_redirections/restore as needed]
    RBP --> RUNB[run_builtin]
    ONE -->|non-NULL| PIPE[execute_pipeline]
    PIPE --> NF{all stages not-found?}
    NF -->|yes| R127[return 127]
    NF -->|no| FORK[run_pipe_step loop + wait_children_last]
```

### 7.2 Single Command Execution

```
┌──────────────────────────────────────────────────────────────┐
│                   SINGLE COMMAND                             │
└──────────────────────────────────────────────────────────────┘
                          │
          ┌───────────────┴───────────────┐
          │     Is it a builtin?          │
          └───────────────┬───────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
         ▼                ▼                ▼
  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
  │ cd/export/  │  │ echo/pwd/   │  │ External    │
  │ unset/exit  │  │ env         │  │ Binary      │
  │ (State-     │  │ (No-state   │  │ (ls, cat)   │
  │  changing)  │  │  builtin)   │  │             │
  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘
         │                │                │
         ▼                ▼                ▼
  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
  │ RUN IN      │  │ Can run in  │  │ MUST fork   │
  │ PARENT      │  │ parent OR   │  │             │
  │ (no fork)   │  │ fork        │  │             │
  └─────────────┘  └─────────────┘  └─────────────┘
```

**Why run cd/export/unset/exit in parent?**

- `cd`: Must change parent's working directory
- `export`: Must modify parent's environment
- `unset`: Must modify parent's environment
- `exit`: Must exit the parent shell

**Note:** For a **single** command, state-changing builtins (`cd`, `export`, `unset`, `exit`) always run in the parent. Other builtins run in the parent **unless** the command has redirections/heredoc — then they go through `execute_external` (fork) like a simple command with redirs.

### 7.3 Single command (actual: `executor/executor.c`)

Redirections use static `backup_fds` / `restore_fds` only when `cmd->redirs` or `heredoc_fd` is set. Empty argv → `run_empty_command`. Builtin path → `run_builtin_command` (may delegate to `execute_external` if redirs + not `must_run_in_parent`). Else → `execute_external` → fork → child `apply_redirections` + `execute_in_child`.

### 7.4 Pipeline Execution

```
Command: ls -la | grep ".c" | wc -l

┌──────────────────────────────────────────────────────────────┐
│  PARENT PROCESS                                              │
│  ─────────────────────────────────────────────────────────── │
│  1. Count commands (3)                                       │
│  2. Create pipes: pipe1[2], pipe2[2]                         │
│  3. Fork child for each command                              │
│  4. Close ALL pipe ends in parent                            │
│  5. waitpid for all children                                 │
│  6. Get exit status from LAST child                          │
└──────────────────────────────────────────────────────────────┘
         │
         ├──────────────────┬──────────────────┐
         ▼                  ▼                  ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   CHILD 1   │     │   CHILD 2   │     │   CHILD 3   │
│   ls -la    │────▶│  grep ".c"  │────▶│   wc -l     │
│             │pipe1│             │pipe2│             │
│ stdout→pipe1│     │stdin←pipe1  │     │stdin←pipe2  │
│             │     │stdout→pipe2 │     │             │
└─────────────┘     └─────────────┘     └─────────────┘
```

**Verified by tests (executor / pipelines):** Single commands: Phase 1 + Hardening (builtins in parent, externals forked). Pipeline stdout: Hardening simple/two/five pipes, pipe with grep/wc -l, pipe builtin echo, pipe with spaces. Pipeline exit: `true | false` → 1, `false | true` → 0. Pipeline + redir and stress (long pipeline, many pipelines, pipe redir combo, export then pipe): no crash. Path: absolute path, command not found (127), directory as cmd (126). See [BEHAVIOR.md](BEHAVIOR.md) §3, §7.

### 7.5 Pipeline (actual: `executor_pipeline.c` + `executor_pipeline_steps.c`)

```mermaid
flowchart LR
    EP[execute_pipeline] --> NF[handle_all_not_found_pipeline]
    NF --> IGN[set_signals_ignore]
    IGN --> LOOP[run_pipeline_loop]
    LOOP --> RPS[run_pipe_step per cmd]
    RPS --> FORK[fork_pipeline_cmd]
    FORK --> CHILD[child: setup_child_fds, apply_redirections, execute_in_child]
    LOOP --> WAIT[wait_children_last]
    WAIT --> INT[set_signals_interactive]
```

No `pids[]` array: parent tracks only `prev_fd` between steps; `waitpid(-1, …)` in `wait_children_last` reaps `n` children and returns the **last** segment’s status.

### 7.6 Command Execution (In Child) — actual: `executor_child_exec.c` `execute_in_child()`

```c
/* executor_child_exec.c */
void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_builtin)
		run_builtin_child(cmd, shell);  /* clean_exit(shell, run_builtin(...)) */
	if (!cmd->argv || !cmd->argv[0])
		clean_exit(shell, 0);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		cmd_not_found(shell, cmd->argv[0]);
	/* PATH unset edge cases, is-directory → 126, then execve */
	execve(path, cmd->argv, shell->envp);
	handle_exec_error(shell, cmd->argv[0]);
}
```

**Supporting files:**
- **`executor_child_format.c`**: `write_err3`, `format_cmd_name_for_error` — errors and `$'…'` display for not found.
- **`executor_pipeline_not_found.c`**: `handle_all_not_found_pipeline` — if every stage is a simple missing-PATH external with no redirs/heredoc, print all errors in parent and return **127** without forking.

### 7.7 Path Resolution (actual: `executor_external.c`)

`find_command_path` uses a **static** buffer (`PATH_MAX`): copy the return value before the next call if you need to keep it. If `cmd` contains `/`, the path is copied as-is (resolution happens at `execve`). Otherwise scan `PATH` colon-separated segments with `stat` (regular file), not `ft_split` + `access` as in older sketches. If `PATH` is missing but the shell **had** `PATH` at startup, a default list is used (`/usr/local/bin:/usr/bin:/bin:.`).

---

## 8. Exit Status Reference (Bash-Aligned)

All exit codes follow the [Bash Reference Manual](https://www.gnu.org/software/bash/manual/html_node/Exit-Status.html) and common shell conventions so that `$?` and scripted behavior match bash. **Verified by:** Phase 1 (exit 0/42/255, exit no args); Hardening §10, §11, §14, §17 (exit 256/257, -1, non-numeric, too many args; 127/126; pipeline last command). See [BEHAVIOR.md](BEHAVIOR.md) §6.

### 8.1 Summary Table

| Scenario                     | Exit Code      | Bash reference / usage                          |
| ---------------------------- | -------------- | ----------------------------------------------- |
| Command success              | `0`            | Normal success                                  |
| Command general error        | `1`            | General failure; builtin "too many args" return |
| Syntax error (shell misuse)  | `2`            | Misuse of shell builtin / syntax error           |
| Permission denied (exec)     | `126`          | File found but not executable                    |
| Command not found            | `127`          | Command not in PATH                             |
| Fatal signal N              | `128 + N`      | Child killed by signal N (e.g. 130 = SIGINT)     |
| Ctrl+C (SIGINT)              | `130`          | `128 + 2`                                       |
| Ctrl+\ (SIGQUIT)             | `131`          | `128 + 3`                                       |
| `exit` with valid arg        | `arg % 256`    | 0–255; out-of-range wraps (e.g. 256 → 0)        |
| `exit` with non-numeric      | `255` (bash) / **`2` (this shell)** | Bash: stderr + exit 255. **We:** same message, **exit 2** (known difference). |
| `exit` with too many args    | (no exit)      | Print error to stderr, return 1, shell continues |

### 8.2 Where We Use Each Code

- **0** – Successful builtin or external command.
- **1** – Builtin failure (e.g. `exit` too many args **returns** 1), redirection failure, or generic error.
- **2** – Syntax error (`syntax_check` / `EXIT_SYNTAX_ERROR`); also **`exit` with non-numeric arg** exits **2** (bash uses **255**).
- **126** – Path is directory or permission denied before/after `execve` (`executor_child_exec.c` `handle_exec_error` / `check_is_dir`).
- **127** – Command not found (`executor_child_exec.c` `cmd_not_found`; pipeline fast path `handle_all_not_found_pipeline` in parent).
- **128 + signal** – Child terminated by signal; e.g. **130** = SIGINT (`executor_external.c` `get_child_status`, pipeline `wait_children_last`).

### 8.3 exit Builtin (Bash Reference)

- **Interactive only:** In an interactive shell, bash prints `"exit\n"` (or `"logout\n"` for login shells) to **stderr** before exiting (see `builtins/exit.def`). We do the same: `ft_putendl_fd("exit", STDERR_FILENO)` when `isatty(STDIN_FILENO)`.
- **Non-numeric argument:** Bash exits with **255** after printing "numeric argument required" to stderr. **We exit 2** (same message; exit code differs — see `exit.c`).
- **Too many arguments:** Bash does not exit; it prints an error and returns 1. We match this.

---

## 9. Built-in Commands (Detailed Specs)

### 9.1 echo [-n] [args...]

```bash
# Behavior:
echo hello world        # "hello world\n"
echo -n hello           # "hello" (no newline)
echo -n -n -n hello     # "hello" (multiple -n same as one)
echo -nnnnn hello       # "hello" (bash accepts this)
echo -n -a hello        # "-a hello\n" (-a not valid, stops -n parsing)
echo ""                 # "\n" (empty string = just newline)
echo                    # "\n" (no args = just newline)
echo -n                 # "" (nothing, not even newline)
```

### 9.2 cd [path]

```bash
# Behavior:
cd /tmp                 # Change to /tmp
cd                      # Change to $HOME
cd -                    # Change to $OLDPWD, print new path
cd ""                   # Error or no-op (bash: no error, stays)
cd nonexistent          # Error: "No such file or directory"

# Must update:
# - PWD environment variable
# - OLDPWD environment variable
# - shell->cwd
```

### 9.3 pwd

```bash
# Behavior:
pwd                     # Print current working directory
# No options needed
# Use getcwd() or shell->cwd
```

### 9.4 export [name[=value]...]

```bash
# Behavior:
export                  # Print all exported vars (sorted, with "declare -x")
export VAR=value        # Set and export
export VAR              # Mark existing var for export (or create empty)
export VAR=             # Set VAR to empty string
export 1VAR=x           # Error: not a valid identifier
export VAR=hello=world  # VAR = "hello=world" (only first = splits)
```

### 9.5 unset [name...]

```bash
# Behavior:
unset VAR               # Remove VAR from environment
unset VAR1 VAR2         # Remove multiple
unset NONEXISTENT       # No error (silent)
unset 1VAR              # Error: not a valid identifier
```

### 9.6 env

```bash
# Behavior:
env                     # Print all environment variables
# No options or arguments
# Only print vars that have been exported
```

### 9.7 exit [n]

Bash reference: message `"exit"` is printed to **stderr** in interactive mode only.

```bash
# Behavior (mostly bash; non-numeric exit uses 2 here, bash uses 255):
exit                    # Exit with last command's status
exit 0                  # Exit with 0
exit 42                 # Exit with 42
exit 256                # Exit with 0 (256 % 256)
exit -1                 # Exit with 255 (two's complement)
exit abc                # Error to stderr: "numeric argument required", then exit 2 (bash: 255)
exit 1 2 3              # Error to stderr: "too many arguments", return 1, do NOT exit
```

### 9.8 Verified by tests (builtins)

All builtin behaviors above are covered by **Phase 1** and **Hardening**:

| Builtin | Phase 1 tests | Hardening tests |
|---------|----------------|------------------|
| **echo** | echo basic, multiple words, -n flag, -n multiple, -nnnnn, no args, empty string, -n only, -n stops at invalid | echo basic, -n, -nnn, -n stops at non-flag, empty, empty string arg, single/double quotes |
| **pwd** | pwd basic | pwd, cd /tmp and pwd, cd HOME |
| **cd** | cd /tmp then pwd, cd HOME, cd nonexistent | cd /tmp and pwd, cd HOME, cd nonexistent, cd with extra args |
| **env** | env contains PATH/HOME/USER | env has PATH/HOME, export sets var then env, unset then env |
| **export** | export no args, export set var, export invalid name | export no args has declare, export sets var, export invalid name, export bad name exit 1 |
| **unset** | unset removes var | unset removes from env |
| **exit** | exit no args, exit 0/42/255 | exit 0/42/255, 256 wraps, no args, non-numeric, too many args no exit; exit -1, 257 wraps, too many args exit 1 |

See [BEHAVIOR.md](BEHAVIOR.md) §5 for input/output examples and exit codes.

---

## 10. Error Messages Format

```c
/* Standard error format (match bash style): */
ft_putstr_fd("minishell: ", 2);
ft_putstr_fd(context, 2);       /* e.g., "cd" or filename */
ft_putstr_fd(": ", 2);
ft_putstr_fd(error_msg, 2);     /* e.g., "No such file or directory" */
ft_putstr_fd("\n", 2);

/* Examples: */
"minishell: cd: /nonexistent: No such file or directory"
"minishell: syntax error near unexpected token `|'"
"minishell: export: `1invalid': not a valid identifier"
"minishell: ./script: Permission denied"
"minishell: nosuchcmd: command not found"
```

---

## 11. Memory Management & Cleanup

### 11.1 Per-Loop Cleanup (actual: `free/free_shell.c`)

```c
void reset_shell(t_shell *shell)
{
    free(shell->input);
    shell->input = NULL;
    free_tokens(shell->tokens);
    shell->tokens = NULL;
    free_commands(shell->commands);
    shell->commands = NULL;
}
```

### 11.2 Exit Cleanup (actual: `free/free_shell.c`)

```c
/* free/free_shell.c: free_all() — used at process exit (e.g. from builtin_exit) */
void free_all(t_shell *shell)
{
    free_tokens(shell->tokens);
    free_commands(shell->commands);
    free_envp(shell->envp);
    free(shell->user);
    free(shell->cwd);
    free(shell->input);
    /* rl_clear_history() is called in builtin_exit before free_all */
}
```

### 11.3 Defensive Free Pattern

```c
void safe_free(void **ptr)
{
    if (ptr && *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}
```

---

## 12. Testing Checklist

Covered by **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** (e.g. `./scripts/run_minishell_tester.sh m`). See [BEHAVIOR.md](BEHAVIOR.md) for expected behavior and test-design guidance.

### 12.1 Basic Commands

- [x] `ls`, `cat`, `echo`, `pwd` work
- [x] Commands with arguments work
- [x] Absolute paths work: `/bin/ls`
- [x] Relative paths work: `./minishell`

### 12.2 Builtins

- [x] `echo` with and without `-n`
- [x] `cd` with path, no args, `-`
- [x] `pwd` prints correct directory
- [x] `export` shows and sets variables
- [x] `unset` removes variables
- [x] `env` shows environment
- [x] `exit` with and without code

### 12.3 Redirections

- [x] `< file` reads from file
- [x] `> file` writes to file (creates/truncates)
- [x] `>> file` appends to file
- [x] `<< EOF` heredoc works
- [x] Multiple redirections work

### 12.4 Pipes

- [x] `ls | cat` works
- [x] `ls | cat | wc` works
- [x] `cat | cat | cat` works
- [x] Pipes with builtins work

### 12.5 Expansion

- [x] `$HOME` expands correctly
- [x] `$?` expands to exit code
- [x] `"$VAR"` expands in double quotes
- [x] `'$VAR'` does NOT expand
- [x] `$UNDEFINED` becomes empty

### 12.6 Signals

- [x] Ctrl+C shows new prompt
- [x] Ctrl+D exits shell
- [x] Ctrl+\ does nothing in prompt
- [x] Ctrl+C during `cat` kills cat

### 12.7 Edge Cases

- [x] Empty input (just Enter)
- [x] Only spaces/tabs
- [x] Unclosed quotes rejected with syntax error (no continuation)
- [x] Invalid pipe syntax error
- [x] Non-existent command error

---

## 13. Implementation Order (Current Status)

Reflects the **built** codebase; phase1 + hardening tests pass.

```
Phase 1: Foundation
├── [x] Shell struct and initialization (core/init.c, structs.h)
├── [x] Main loop with readline (TTY) / read_line_stdin (non-TTY) (src/main.c)
├── [x] Basic signal handling (signals/signal_handler.c, signal_utils.c)
└── [x] Builtins (echo, cd, pwd, export, unset, env, exit)

Phase 2: Lexer & Parser
├── [x] Tokenizer (tokenizer/tokenizer.c, tokenizer_ops.c, tokenizer_handlers.c, tokenizer_quotes.c)
├── [x] Quote handling (unclosed quote -> syntax error, no continuation)
├── [x] Syntax validation (parser/parser_syntax_check.c)
└── [x] Command table construction (parser/parser.c, add_token_to_cmd.c, argv_build.c)

Phase 3: Expander
├── [x] Variable expansion (tokenizer/expansion.c, tokenizer/expansion_utils.c — $VAR, $?)
├── [x] Exit status expansion ($?)
├── [x] Quote removal (during tokenization)
└── [x] Edge case handling (e.g. $ at end, invalid names)

Phase 4: Executor (Simple)
├── [x] Single external command execution (executor/executor_external.c)
├── [x] Path resolution (find_command_path in executor_external.c)
├── [x] Single builtin with redirections (run_builtin_command / execute_external, apply_redirections)
└── [x] File redirections (executor/executor_utils.c, apply_one_redir, heredoc_fd)

Phase 5: Pipes & Heredoc
├── [x] Pipeline execution (executor/executor_pipeline.c)
├── [x] Heredoc implementation (parser/heredoc.c, parser/heredoc_utils.c)
├── [x] Multiple redirections (cmd->redirs list, left-to-right)
└── [x] Signal handling in children (wait_children_last, exit codes 128+N)

Phase 6: Polish & Refactor
├── [x] Error messages (minishell: cmd: msg style)
├── [x] Memory cleanup (free/free_shell.c, free/free_runtime.c, reset_shell)
├── [x] Edge case handling (hardening tests pass)
├── [x] Executor split: child_exec, child_format, pipeline_steps, pipeline_not_found
├── [x] Exit utils extracted: parse_exit_value in exit_utils.c
├── [x] Pipeline barrier for stderr ordering (barrier_write_fd in t_shell)
└── [ ] Norminette / 42 compliance (project-specific)
```

---

## 14. Related documentation

| Document | Purpose |
|----------|---------|
| **[BEHAVIOR.md](BEHAVIOR.md)** | Test-backed behavior: redirections, pipes, expansion, builtins, exit codes, path resolution, input resilience. Use for evaluation and debugging. |
| **[DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md)** | **Data model:** why we chose each struct/enum. **Function reference:** every function by file with one-line description; Mermaid call flow. |
| **[TECHNICAL_DECISIONS.md](TECHNICAL_DECISIONS.md)** | Record of what we changed and why: data, functions, defensive/bug prevention, 42 constraints; **recent critical fixes** table. |
| **[42_tester_failures.md](42_tester_failures.md)** | Mandatory tester CI, session notes (e.g. 2026-03-19 fixes: `add_token`, USER, PATH, export/unset). |
| **README.md** | Project overview, build, usage, how to run tests. |

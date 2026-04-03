# Minishell Project Architecture (Defensive Programming)

> **Philosophy:** Defensive programming means we validate every input, handle every error case explicitly, and never assume success. We use bash as our reference implementation but only implement what the 42 subject requires.
>
> **Test-backed behavior:** Primary harness is **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** (`tester.sh`, mandatory mode `m`). For expected I/O and exit codes, see **[BEHAVIOR.md](BEHAVIOR.md)**.
>
> **Figures (Mermaid):** §0.1 (harness), §0.3 (source graph), §0.4 (input→execute gate), §1.4 (Ctrl+C sequence), §2 (REPL), §3.2.1 (tokenizer loop), §4.1 (expansion vs heredoc), §5.2 (`parse_input` + token walk), §6.2 (heredoc), §7.1 / §7.2 / §7.5 (executor), §8.2 (`last_exit` writers). For dedicated signal flowcharts and tables, see **[SIGNAL.md](SIGNAL.md)**.

---

## 0. Project Status & Built Implementation

This section reflects the **actual codebase** as built: source layout, data flow, and test status.

### 0.1 Test harness (current repo)

| Where | What runs | Notes |
|-------|-----------|--------|
| **Local (Docker)** | `./scripts/run_minishell_tester.sh [mode]` | Clones [LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester) into the dev container (`/root/42_minishell_tester`), builds `/app`, then `tester.sh --no-update <mode>`. Default mode `m` = mandatory. Common: `m`, `vm`, `b`, `ne`, `a`, `va` (see script header). |
| **GitHub Actions** | `.github/workflows/test.yaml` + `regression_test.yaml` + `funcheck.yaml` | Same upstream: `git clone https://github.com/LeaYeh/42_minishell_tester.git`. Regression matrix: `m` (mandatory), `b` (bonus), `ne` (empty env). Crash: `a` / `a --no-env`. Valgrind: one job per `cmds/**/*.sh` with `tester.sh va`. `funcheck.yaml` runs the funcheck allowed-function checker. |
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
| `src/` | `main.c` only — `main`: `init_shell`, `shell_loop`, teardown (`rl_clear_history`, `free_all`, close std fds, return `last_exit`). |
| `src/init/` | **`init_env.c`** (`get_env_value`, `init_shell_identity`, `init_runtime_fields`), **`init_shell.c`** (`init_shell`, SHLVL bump), **`repl_process.c`** (`process_input`), **`repl_loop.c`** (`shell_loop`; static `read_input`, `build_prompt`, `read_line_stdin`, `repl_after_read`) |
| `src/utils/` | `msh_string_append_char`, `msh_word_append_expanded`, `msh_strarray_dup`, `msh_char_buffer_realloc`, `msh_string_expand_scan`, `msh_stdin_read_line` (shared string/buffer/line helpers) |
| `src/free/` | Memory cleanup: `free_utils.c`, `free_runtime.c`, `free_shell.c` |
| `src/signals/` | Signal handlers and readline hook |
| `src/tokenizer/` | Tokenizer: tokenizer.c, tokenizer_loop.c, expansion, quote/operator handlers, utils |
| `src/parser/` | Parser: **`parse_input.c`**, **`parse_syntax.c`**, **`parse_pipeline.c`**, **`parse_attach_token.c`**, **`parse_redir.c`**, **`parse_finalize.c`**, **`heredoc_collect.c`**, **`heredoc_io.c`**, **`heredoc_expand.c`** |
| `src/executor/` | **`exec_*`:** `exec_dispatch.c`, `exec_redir.c`, `exec_external.c`, `exec_wait.c`, `exec_notfound.c`, `exec_child.c`, **`exec_pipeline_nf.c`** (`pip_all_nf`), `exec_pipeline.c`, `exec_pipe_step.c` — public API unchanged (`run_commands`, `apply_redirs`, …). |
| `src/builtins/` | Builtin commands and dispatcher, export_print, exit_utils |

```mermaid
graph TB
    subgraph Entry
        main[main.c]
    end
    subgraph init
        init_sh[init/init_shell.c]
        init_u[init/init_env.c]
        repl[init/repl_loop.c]
        repl_p[init/repl_process.c]
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
        tok_loop[tokenizer_loop.c]
        tok_ops[tokenizer_ops.c]
        tok_handlers[tokenizer_handlers.c]
        tok_quotes[tokenizer_quotes.c]
        expansion[expansion.c]
        expansion_word[expansion_word.c]
        expansion_utils[expansion_utils.c]
    end
    subgraph Parser
        parser[parse_input.c]
        syntax[parse_syntax.c]
        add_token[parse_attach_token.c]
        argv_build[parse_finalize.c]
        heredoc[heredoc_collect.c]
        heredoc_input[heredoc_io.c]
        heredoc_utils[heredoc_expand.c]
    end
    subgraph Executor
        exe_m[exec_dispatch.c]
        exe_redir[exec_redir.c]
        exe_external[exec_external.c]
        exe_nf[exec_notfound.c]
        exe_child[exec_child.c]
        exe_pnf[exec_pipeline_nf.c]
        exe_pl[exec_pipeline.c]
        exe_ps[exec_pipe_step.c]
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
    main --> init_sh
    main --> repl
    init_sh --> init_u
    repl --> repl_p
    repl_p --> tok
    tok --> parser
    parser --> exe_m
    exe_m --> dispatcher
    exe_m --> exe_redir
    exe_m --> exe_external
    exe_m --> exe_pl
    exe_pl --> exe_pnf
    exe_pl --> exe_ps
    exe_pnf --> exe_nf
    exe_ps --> exe_child
    exe_external --> exe_child
    exe_child --> exe_nf
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

**Gate:** **`tokenize_input` → … → `run_commands`** run only when **`shell_loop`** calls **`process_input`**, i.e. when **`shell->input[0] != '\0'`** (see **§2**). Empty lines skip this entire chain; **`reset_shell`** still runs.

```mermaid
flowchart LR
    A[readline / read_line_stdin] --> Q{"input[0] non-zero?"}
    Q -->|no| R[reset_shell]
    Q -->|yes| B[tokenize_input]
    B --> C[parse_input]
    C --> D[process_heredocs]
    D --> E[run_commands]
    E --> R
    B -.-> T[(tokens)]
    C -.-> H[(commands)]
    E -.-> I["empty / builtin / external / pipeline (child or parent)"]
```

- **`repl_loop.c`** (src/init/): `shell_loop` → `check_signal_received` → `read_input` → if **`shell->input[0]`** then **`process_input`** in **`repl_process.c`** (tokenize → parse → heredocs → execute) → `reset_shell`; non-TTY may **`break`** on syntax error (see **§2**). **`main.c`** only calls **`init_shell`**, **`shell_loop`**, and teardown.
- **Tokenizer** (src/tokenizer/): `tokenize_input()` in `tokenizer.c`; uses `tokenizer_handlers.c`, `tokenizer_quotes.c`, `expansion.c`, and `tokenizer_ops.c`.
- **Parser** (src/parser/): `parse_input()` in `parse_input.c`; `syntax_check()` in `parse_syntax.c`; `finalize_cmds()` in `parse_finalize.c` builds `argv` and sets `is_builtin`.
- **Executor** (src/executor/): **`run_commands()`** in **`exec_dispatch.c`** — empty
  argv, **`run_empty_command`**; else single command:
  **`run_single_builtin`** (parent with optional `dup`/`apply_redirs`/
  `restore_stdio_fds`, or **`run_external`** if builtin has redirs and type is
  not cd/export/unset/exit) or **`run_external`**; pipeline ->
  **`run_pip()`** -> **`pipe_step`** /
  **`wait_pipes`**; child path -> **`run_in_child()`** in
  **`exec_child.c`**.

---

## 1. Global State & Signal Handling

### 1.1 The only global: `g_signum`

```c
/* signals/signal_handler.c — ONLY global in the project */
volatile sig_atomic_t	g_signum = 0;
```

| `g_signum` | Meaning |
| ---------- | ------- |
| `0` | No signal pending |
| `SIGINT` | Ctrl+C seen by handler; cleared after `check_signal_received()` or when the readline hook consumes it |

**`last_exit = XSINT`:** Set in **`check_signal_received()`** (`signal_utils.c`). **`XSINT`** is **`128 + SIGINT`** (**130**) in **`includes/defines.h`**. The async handler does **not** set `t_shell->last_exit`.

**Rules:** No structs/pointers in globals; **never** use `t_shell` inside a handler. The SIGINT handler only sets **`g_signum`** and **`write(STDOUT_FILENO, "\n", 1)`** (async-signal-safe).

**Why `volatile sig_atomic_t`:** The compiler must reload the value; writes from the handler must be atomic.

### 1.2 Bash-oriented behavior (user-visible)

| Signal / input | At prompt (interactive) | While external/pipeline child runs | During heredoc input |
| -------------- | ------------------------ | ----------------------------------- | -------------------- |
| **SIGINT** (Ctrl+C) | New line, `$? = XSINT` (130), new prompt | Child receives default SIGINT; parent ignores while in **`waitpid`** | Loop stops, `process_input` sets **`XSINT`** (see §6; detail in [SIGNAL.md](SIGNAL.md)) |
| **SIGQUIT** (Ctrl+\) | Ignored (`SIG_IGN`) | Child: default → may print “Quit (core dumped)” | Ignored |
| **EOF** (Ctrl+D) | `readline` returns NULL → exit shell | N/A | Line ends / EOF handling in **`read_heredoc_line`** |

See **[BEHAVIOR.md](BEHAVIOR.md)** for tables and tester-oriented notes (e.g. SIGPIPE / stderr on Linux).

### 1.3 Interactive installation (`set_signals_interactive`)

**Call site:** `main()` after `init_shell()` (`signal_handler.c`).

| Signal | Action |
| ------ | ------ |
| **SIGINT** | `interactive_sigint_handler`, **`SA_RESTART`**, **`sa_mask`** includes **SIGQUIT** (no re-entrancy race on Ctrl+\) |
| **SIGQUIT** | `SIG_IGN` |
| **SIGTERM** | `SIG_IGN` |
| **SIGPIPE** | `SIG_IGN` (children inherit; common on Linux systemd sessions) |

**TTY + readline:** `main()` sets **`rl_event_hook = readline_event_hook`**. While readline is active, the hook sees **`g_signum == SIGINT`**, clears the line, **`rl_on_new_line()`**, **`rl_done = 1`**, so readline returns.

### 1.4 Ctrl+C at the prompt (control flow)

1. Handler: **`g_signum = SIGINT`**, **`write(1, "\n", 1)`**.
2. **`readline_event_hook`**: discard buffer, end readline.
3. **`read_input()`** calls **`check_signal_received(shell)`**: **`last_exit = XSINT`**, **`g_signum = 0`**, return **`RL_SIG`** → **`shell_loop`** **`continue`** (no `process_input` on that line).
4. **`shell_loop`** also calls **`check_signal_received`** at the **start** of each iteration.

```mermaid
sequenceDiagram
    participant H as interactive_sigint_handler
    participant RL as readline + event_hook
    participant RI as read_input
    participant CS as check_signal_received
    H->>H: g_signum=SIGINT, write newline
    RL->>RL: hook clears line, rl_done
    RL->>RI: readline returns
    RI->>CS: after line read
    CS->>CS: last_exit=XSINT, g_signum=0
```

### 1.5 Dispositions around `fork` / `wait`

| Phase | Function | Effect |
| ----- | -------- | ------ |
| Child after **`fork`** | **`set_signals_default`** | SIGINT, SIGQUIT, SIGPIPE, SIGTERM → **default** (`exec_external.c`, **`exec_pipe_step.c`**) |
| Parent while waiting | **`set_signals_ignore`** | SIGINT, SIGQUIT → **ignore** so the waiting parent is not torn down by the same keystroke |
| After wait returns | **`set_signals_interactive`** | Restore §1.3 |

**Pipeline:** **`run_pip`** calls **`set_signals_ignore`**, runs
**`spawn_pipes`**, **`wait_pipes`**, then
**`set_signals_interactive`**. **Single external:** same pattern inside
**`run_external`** around **`waitpid`**.

### 1.6 `t_shell` (no further globals)

All other state is in **`t_shell`** (`includes/structs.h`). **Env, cwd, tokens, commands, input:** filled by init/parser/tokenizer. **Extra fields:**

- **`had_path`:** PATH was present when the shell started (PATH resolution).
- **`path_unset`:** set once `unset PATH` is executed; influences PATH fallback.
- **`word_quoted` / `hd_mod`:** tokenizer flags (quoted WORD; no `$` in `<<` delimiter).

Full **init** order: **§0.3** (`init_shell` / `init_runtime_fields`).

---

## 2. Main Loop (REPL Cycle)

**Implementation:** `src/init/repl_loop.c` → **`shell_loop()`** → **`read_input()`** → (optional) **`process_input()`** in **`src/init/repl_process.c`**. Entry point from **`main.c`** is **`shell_loop(&shell)`** after **`init_shell`** (`init/init_shell.c`).

**Input source:** When stdin is a **TTY**, `read_input()` uses **`readline(prompt)`** after **`build_prompt(shell)`** (history, line editing). When stdin is **not** a TTY (e.g. **42_minishell_tester**), it uses **`read_line_stdin()`** (static wrapper → **`ft_read_stdin_line`** in `utils/msh_stdin_read_line.c`): byte **`read(STDIN_FILENO, &c, 1)`** until **`'\n'`** or **EOF**—no prompt, no readline. The stored string **does not** include the newline. If **EOF** is read after at least one character without a newline, that partial line is still returned; the next read may see empty buffer → **EOF** path.

**`read_input()` return values** (macros in **`defines.h`**; only `shell_loop` branches on them):

| Return | Macro | Meaning | `shell_loop` action |
|--------|-------|---------|---------------------|
| **0** | **`RL_EOF`** | **EOF** (`NULL` from readline / stdin path) | **TTY:** **`ft_printf("exit\n")`** (stdout) then **`break`**. **Non-TTY:** **`break`** without printing. |
| **-1** | **`RL_SIG`** | **`check_signal_received()`** inside `read_input` fired; **`shell->input`** freed and cleared | **`continue`** (no `process_input`, no `reset_shell` for that iteration). |
| **`OOM`** | **`OOM`** | e.g. **`build_prompt`** / stdin line allocation failure | **`last_exit = FAILURE`**, **`reset_shell`**, **`break`** (leave REPL toward **`free_all`**). |
| **1** | **`RL_LN`** | A non-**NULL** line was read | **`repl_after_read`**: if **`shell->input[0] != '\0'`** → **`process_input`**, else skip it; then **`reset_shell`**; non-TTY + syntax error may **`break`**. |

**Inside `read_input()`** (after a line pointer is obtained): **`check_signal_received(shell)`** runs for **every** non-**NULL** line (**empty** `""` included) before returning **`RL_LN`**—same mechanism as Ctrl+C at the prompt (§1.4). **`build_prompt()`** failure returns **`OOM`** (not EOF).

**After `process_input()`** (when it was called): **`repl_after_read`** sets **`syntax_err`** if **`!shell->cmds && shell->last_exit == XSYN`** (e.g. unclosed quote). **`reset_shell()`** still runs. If **stdin is not a TTY** and **`syntax_err`**, the loop **`break`**s (tester-style input stops on syntax error).

> **Implementation detail (non-TTY):** **`reset_shell()`** does **not** reset **`last_exit`**. After **`reset_shell`**, **`commands`** is always **NULL**, so **`!shell->cmds && shell->last_exit == XSYN`** stays true on the **next** line even if **`process_input`** is skipped (e.g. empty line). The loop then **`break`**s—useful for scripted input after a syntax error; interactive TTY users are unaffected.

**`process_input()`** (`repl_process.c`): **`tokenize_input`** → **`parse_input`** → if **`!shell->cmds`** return; else **`process_heredocs`** — on failure, if **`g_signum == SIGINT`** then **`last_exit = XSINT`**, else **`last_exit = FAILURE`**, and return **without** **`run_commands`**; else **`last_exit = run_commands(shell)`**.

**Program exit:** **`main()`** returns **`shell.last_exit`** after **`rl_clear_history()`**, **`free_all()`**, and closing std fds.

```mermaid
flowchart TD
    START([shell_loop iteration]) --> CHECK1["check_signal_received(shell)"]
    CHECK1 --> READ["read_input(shell)"]
    READ --> RVAL{read_input return}
    RVAL -->|RL_EOF| EXIT["break — leave REPL"]
    RVAL -->|RL_SIG| START
    RVAL -->|OOM| OOMH["FAILURE + reset_shell + break"]
    OOMH --> EXIT
    RVAL -->|RL_LN| INEMPTY{"input[0] non-zero?"}
    INEMPTY -->|no| RESET["reset_shell — empty line"]
    INEMPTY -->|yes| PROC["process_input — tokenize, parse, heredocs, execute"]
    PROC --> RESET
    RESET --> SYNTAX{"!isatty(stdin) && syntax_err<br/>(no commands && last_exit==XSYN)"}
    SYNTAX -->|yes| EXIT
    SYNTAX -->|no| START
```

| Step | Code / behavior |
|------|------------------|
| 1 | Top of loop: **`check_signal_received(shell)`** — if **`g_signum == SIGINT`**, set **`last_exit = XSINT`**, clear **`g_signum`** (§1). |
| 2 | **`read_input()`**: TTY → **`build_prompt`**, **`readline`**; non-TTY → **`read_line_stdin()`** → **`ft_read_stdin_line`**. |
| 3 | **`read_input`:** **`!shell->input`** → **`RL_EOF`** (see table above). |
| 3b | **`read_input`:** **`OOM`** → **`last_exit = FAILURE`**, **`reset_shell`**, **break** (same exit path as EOF toward **`main`**). |
| 4 | **`read_input`:** **`check_signal_received`** on non-**NULL** line → may return **`RL_SIG`**. |
| 5 | **`repl_after_read`:** only if **`shell->input[0]`** → **`process_input()`** (tokenize → parse → heredocs → execute). |
| 6 | **Readline history:** **`add_history(shell->input)`** in **`handle_end_of_string()`** (`tokenizer_handlers.c`) only if **`isatty(STDIN_FILENO)`** and **`shell->input[0]`** (non-empty line reached EOL without unclosed quote). Empty lines never enter tokenization, so no history entry. |
| 7 | **`reset_shell(shell)`** frees tokens, commands, input string. |
| 8 | Non-TTY **+** syntax error (**`last_exit == XSYN`** and no commands) → **break** loop. |

---

## 3. Tokenizer (tokenization)

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

### 3.2 Tokenizer state machine

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

### 3.2.1 Tokenizer loop (`tokenizer_loop.c`)

**`tokenize_input()`** (`tokenizer.c`) drives one pass over **`shell->input`** via **`tokenizer_loop()`** (`tokenizer_loop.c`). **States** in code: **`ST_NORMAL`**, **`ST_SQUOTE`**, **`ST_DQUOTE`** (`structs.h`). Handlers return **`TOK_N`** / **`TOK_Y`** (not handled / handled) or **`OOM`**; see **`includes/defines.h`**.

```mermaid
flowchart TD
    L([tokenizer_loop]) --> E{end of string?}
    E -->|yes| EOS[handle_end_of_string → history / unclosed quote]
    E -->|no| Q[handle_quotes_and_expand]
    Q -->|handled| L
    Q -->|not| BS[handle_backslash]
    BS -->|handled| L
    BS -->|not| OP[handle_operator PIPE/redirs]
    OP -->|handled| L
    OP -->|not| WS[handle_whitespace → flush word]
    WS -->|handled| L
    WS -->|not| NORM[process_normal_char]
    NORM --> L
```

After the loop: **`tokenizer_end()`** (`tokenizer.c`, static) runs **`flush_word`** if **`ST_NORMAL`**; else frees tokens on quote error; always frees **`shell->input`**.

### 3.3 Syntax Error Detection (Defensive Checks)

**Error: Unclosed Quotes**

```bash
$ echo "hello        # bash: unexpected EOF while looking for matching `"'
$ echo 'hello        # bash: unexpected EOF while looking for matching `''
```

**Our behavior:** Print error, set `last_exit = XSYN`, do NOT execute.

**Error: Invalid Pipe Usage**

```bash
$ | ls              # bash: syntax error near unexpected token `|'
$ ls |              # bash: syntax error near unexpected token `newline'
$ ls || cat         # We don't handle || (logical OR) - treat as syntax error
$ ls | | cat        # bash: syntax error near unexpected token `|'
```

**Our behavior:** Print `minishell: syntax error near unexpected token`, set `last_exit = XSYN`.

**Error: Invalid Redirection**

```bash
$ ls >              # bash: syntax error near unexpected token `newline'
$ ls > > file       # bash: syntax error near unexpected token `>'
$ ls < >            # bash: syntax error near unexpected token `>'
```

### 3.4 Syntax Validation (actual: `parse_syntax.c`)

**API:** **`syntax_check(t_list *lst)`** — each node’s **`content`** is **`t_token *`**. Leading **PIPE** → error; **PIPE** cannot be last or doubled; each redirection must be followed by a **WORD** (else **`"newline"`** or the next token’s type string). See the file for **`check_redir_syntax`** and **`syntax_error`**.

```c
/* Sketch — real code walks t_list, not token->next */
int	syntax_check(t_list *lst)
{
	t_list	*node;
	t_token	*token;

	if (!lst)
		return (SUCCESS);
	token = lst->content;
	if (token->type == PIPE)
		return (syntax_error("|"));
	node = lst;
	while (node)
	{
		token = node->content;
		if (token->type == PIPE
			&& (!node->next
				|| ((t_token *)node->next->content)->type == PIPE))
			return (syntax_error("|"));
		if (is_redirection(token->type) && check_redir_syntax(node))
			return (FAILURE);
		node = node->next;
	}
	return (SUCCESS);
}
```

### 3.5 Verified by tests (tokenizer + syntax)

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
│  • `exp_unq()` splits on spaces/tabs        │
│  • Quoted segments use `exp_q_cat()` (no split) │
│  • Bash IFS is not implemented; split is whitespace-only    │
└──────────────────────────────────────────────────────────────┘
```

```mermaid
flowchart LR
    subgraph tok["During tokenization"]
        A[$VAR / $? / ~] --> B[Quoted vs unquoted paths]
        B --> C[exp_q_cat / unquoted]
        C --> D[Word boundary → flush token]
    end
    subgraph hd["Heredoc body parser/heredoc_expand"]
        H1[expand if delimiter unquoted] --> H2[write to pipe]
    end
```

**Tilde `~`:** handled in **`exp_tilde()`** when not in heredoc mode (same tokenizer pass as **`$`**).

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

Expansion runs during **tokenization** (see `tokenizer/expansion.c`, `tokenizer/expansion_utils.c`). Heredoc expansion is in `parser/heredoc_expand.c` (quoted delimiter → no expand). See [BEHAVIOR.md](BEHAVIOR.md) §4.

---

## 5. Parser (Command Table Construction)

### 5.1 Command Structure (actual: `includes/structs.h`)

```c
/* args and redirs use libft t_list * — no embedded *next on the structs */

typedef struct s_arg
{
    char    *value;
}   t_arg;

typedef struct s_redir
{
    char    *file;      /* filename for < > >> */
    int      fd;        /* dup2 target: STDIN_FILENO or STDOUT_FILENO */
    int      append;    /* 1 for >>; 0 for < or > */
}   t_redir;

typedef struct s_command
{
    t_list  *args;       /* t_list of t_arg *; finalize_cmds builds argv */
    char   **argv;       /* ["cmd", "arg", NULL] for execve — built by finalize_cmds */
    t_list  *redirs;     /* t_list of t_redir * (< > >>; applied left-to-right) */
    int      hd_fd;      /* read end of heredoc pipe (-1 if none) */
    char    *hd_delim;   /* latest << delimiter (last wins per command) */
    int      hd_quoted;  /* delimiter was quoted → no expansion in body */
    int      stdin_last; /* STDIN_LAST_* — tracks < vs << source order for apply_redirs */
    int      is_builtin; /* set by finalize_cmds via get_builtin_type */
}   t_command;
```

> **Note:** `2>` (stderr redirect) is **not implemented** — `t_redir.fd` only holds `STDIN_FILENO` or `STDOUT_FILENO` in practice. Pipeline ordering uses `t_list *` from libft; commands are not linked via embedded `*next`.

- **Parser** fills `args` and `redirs`; **parse_finalize.c** `finalize_cmds()` (via static `finalize_argv`) builds `argv` and sets `is_builtin`.

### 5.2 Parsing Flow (actual: `parser/parse_input.c`, `parser/parse_pipeline.c`, `parser/parse_attach_token.c`, `parser/parse_redir.c`)

```mermaid
flowchart TD
    PI[parse_input] --> T0{tokens?}
    T0 -->|no| NULL[commands = NULL, return]
    T0 -->|yes| SY[syntax_check]
    SY -->|FAILURE| E2[last_exit = XSYN, free tokens, return]
    SY -->|SUCCESS| PT[build_command_list → t_command list]
    PT -->|NULL| E1[last_exit = FAILURE]
    PT -->|ok| FIN[finalize_cmds]
    FIN --> ARGV[finalize_argv + is_builtin per cmd]
```

- **parser/parse_input.c**: **`parse_input()`** — **`syntax_check`** first; on success **`build_command_list()`** (`parse_pipeline.c`) walks tokens; each **`parse_token_step()`**: on **`PIPE`** append new **`t_command`**, else **`add_token_to_command()`** (WORD → args, redirs → **`parse_redir_token_pair`** in **`parse_redir.c`** / **`handle_heredoc_token`**).
- **parser/parse_finalize.c**: **`finalize_cmds()`** → **`finalize_argv()`** (args list → **`argv[]`**), then **`get_builtin_type(cmd->argv[0]) != B_NONE`** → **`cmd->is_builtin`**.

```mermaid
flowchart LR
    subgraph walk["build_command_list walk"]
        TK[token stream] --> STEP[parse_token_step]
        STEP -->|PIPE| NC[new t_command]
        STEP -->|WORD/redir| ADD[add_token_to_command]
    end
    NC --> STEP
    ADD --> STEP
```

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
    B_NONE = 0,
    B_ECHO,
    B_CD,
    B_PWD,
    B_EXPORT,
    B_UNSET,
    B_ENV,
    B_EXIT,
    B_COUNT
}   t_builtin;

/* t_builtin_reg { name, run } in structs.h; static tab inside builtin_registry() */
t_builtin   get_builtin_type(char *cmd);
int         run_builtin(char **argv, t_shell *shell);
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
│     c. Store read end in cmd->hd_fd                     │
└──────────────────────────────────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  3. Last heredoc FD becomes stdin for command                │
└──────────────────────────────────────────────────────────────┘
```

```mermaid
flowchart LR
    PH[process_heredocs] --> E[each << : pipe in parent]
    E --> L[read_heredoc_line loop]
    L -->|delimiter| F["hd_fd on command"]
    L -->|SIGINT| X[FAILURE → last_exit 130 in repl_process.c]
    L -->|EOF| W[print_heredoc_eof_warning → continue]
    F --> EX[execute: dup2 hd_fd → stdin]
```

### 6.3 Heredoc + Signals

**Implementation:** `parser/heredoc_collect.c` — loop calls **`heredoc_read_line()`** in **`heredoc_io.c`**: TTY → **`readline("> ")`**, else **`ft_read_stdin_line(..., 1)`** (OOM may set **`shell->oom`**). After each line, **`g_signum == SIGINT`** → close pipe ends, return **`FAILURE`** (`process_input` sets **`last_exit = XSINT`**). EOF without delimiter → **`print_heredoc_eof_warning()`**; body lines go through **`write_heredoc_line()`** when expanding.

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

**Implementation:** `executor/exec_dispatch.c` (`run_commands`, static
`run_empty_command` / `run_single_builtin`), `executor/exec_redir.c`
(`apply_redirs`), `executor/exec_external.c` (`run_external`,
`resolve_cmd_path`), `executor/exec_pipeline_nf.c` (`pip_all_nf`),
`executor/exec_pipeline.c` + `executor/exec_pipe_step.c` (`run_pip`,
`pipe_step`; **`sync_fd` inactive**), `executor/exec_child.c`
(`run_in_child`), `executor/exec_notfound.c` (`put_cmd_not_found`).

### 7.1 Decision Tree (real code path)

```mermaid
flowchart TD
    EC[run_commands]
    EC --> NO_CMD{"cmds list set?"}
    NO_CMD -->|NULL| R0[return SUCCESS]
    NO_CMD -->|non-NULL| ONE{"next pipeline segment?"}
    ONE -->|NULL| EMP{"argv missing or argv0 empty?"}
    EMP -->|yes| EMPTY[run_empty_command redir only]
    EMP -->|no| BUILTIN{"is_builtin?"}
    BUILTIN -->|yes| RB[run_single_builtin]
    BUILTIN -->|no| EXT[run_external]
    RB --> RBX{redirs and not parent-only builtin?}
    RBX -->|yes| EXT
    RBX -->|no| RBP[dup2/apply_redirs/restore as needed]
    RBP --> RUNB[run_builtin]
    ONE -->|non-NULL| PIPE[run_pip]
    PIPE --> NF{all stages not-found?}
    NF -->|yes| R127[return XNF]
    NF -->|no| FORK[pipe_step loop + wait_pipes]
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

**Note:** For a **single** command, state-changing builtins (`cd`, `export`, `unset`, `exit`) always run in the parent. Other builtins run in the parent **unless** the command has redirections/heredoc — then they go through `run_external` (fork) like a simple command with redirs.

```mermaid
flowchart TD
    SC[Single command] --> MT{parent-only builtin?}
    MT -->|cd export unset exit| PAR[run_builtin in parent]
    MT -->|no| RD{redirs or hd_fd?}
    RD -->|yes| FK[fork via run_external]
    RD -->|no| PAR2[run_builtin in parent]
    FK --> CH[child: apply_redirs, run_builtin_in_child or execve]
```

### 7.3 Single command (actual: `executor/exec_dispatch.c`)

Redirections use static `backup_stdio_fds` / `restore_stdio_fds` only when
`cmd->redirs` or `hd_fd` is set. Empty argv -> `run_empty_command`.
Builtin path -> `run_single_builtin` (may delegate to `run_external` if redirs
and not cd/export/unset/exit). Else -> `run_external` -> fork -> child
`apply_redirs` + `run_in_child`.

**Algorithm choice (why this is good):**

- **Decision tree first, syscalls second:** we decide parent vs child execution
  before touching FDs, which keeps stateful builtins safe.
- **Copy-apply-restore for parent builtins:** use `dup` + `dup2` to prevent
  parent stdio corruption when builtins have redirections.
- **Fail-fast strategy:** any redirection failure returns quickly with a clear
  status instead of continuing with partially modified FDs.

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

**Verified by tests (executor / pipelines):** Single commands: Phase 1 + Hardening (builtins in parent, externals forked). Pipeline stdout: Hardening simple/two/five pipes, pipe with grep/wc -l, pipe builtin echo, pipe with spaces. Pipeline exit: `true | false` → 1, `false | true` → 0. Pipeline + redir and stress (long pipeline, many pipelines, pipe redir combo, export then pipe): no crash. Path: absolute path, command not found (`XNF`), directory as cmd (`XNX`). See [BEHAVIOR.md](BEHAVIOR.md) §6, §10.

### 7.5 Pipeline (actual: `exec_pipeline.c` + `exec_pipe_step.c`)

```mermaid
flowchart LR
    EP[run_pip] --> NF[pip_all_nf]
    NF --> IGN[set_signals_ignore]
    IGN --> LOOP[spawn_pipes]
    LOOP --> RPS[pipe_step per cmd]
    RPS --> FORK[fork_pipe_child]
    FORK --> CHILD[child: setup_pipe_child_fds, apply_redirs, run_in_child]
    LOOP --> WAIT[wait_pipes]
    WAIT --> INT[set_signals_interactive]
```

No `pids[]` array: parent tracks only `prev_fd` between steps;
`waitpid(-1, ...)` in `wait_pipes` reaps `n` children and
returns the **last** segment's status.

**Algorithm choice (why this is good):**

- **Streaming pipeline construction:** `pipe_step` creates one stage at a time.
  This avoids preallocating a pid/fd matrix and keeps code Norm-friendly.
- **Last-command status policy:** explicitly tracks `last_pid`, matching bash
  rule for pipeline exit status.
- **EINTR-aware wait loop:** interrupted waits retry instead of miscounting
  children, improving robustness under signal load.

### 7.6 Command Execution (In Child) — actual: `exec_child.c` `run_in_child()`

```c
/* exec_child.c — sketch */
void	run_in_child(t_command *cmd, t_shell *shell)
{
	if (cmd->is_builtin)
		run_builtin_in_child(cmd, shell);
	if (!cmd->argv || !cmd->argv[0])
		exit_norl(shell, SUCCESS);
	path = resolve_cmd_path(cmd->argv[0], shell);
	if (!path)
		child_exit_not_found(shell, cmd->argv[0]); /* XNF */
	/* stat: is directory → XNX */
	execve(path, cmd->argv, shell->envp);
	/* ENOENT / permission → XNF or XNX via child_abort_msg */
}
```

**Supporting files:**
- **`exec_notfound.c`**: **`put_cmd_not_found`** — not-found line to stderr
  (`$'…'` when name has control bytes).
- **`exec_pipeline_nf.c`**: **`pip_all_nf`** — if every stage is a simple missing-PATH external with no redirs/heredoc, print all errors in parent; **`run_pip`** returns **`XNF`** without forking that pipeline.

### 7.7 Path Resolution (actual: `exec_external.c`)

`resolve_cmd_path` uses a **static** buffer (`PATH_MAX`): copy the return value before the next call if you need to keep it. If `cmd` contains `/`, the path is copied as-is (resolution happens at `execve`). Otherwise scan `PATH` colon-separated segments with `stat` (regular file), not `ft_split` + `access` as in older sketches. If `PATH` is missing and (`had_path == 1` or `path_unset == 0`), a default list is used (`/usr/local/bin:/usr/bin:/bin:.`).

---

## 8. Exit Status Reference (Bash-Aligned)

All exit codes follow the [Bash Reference Manual](https://www.gnu.org/software/bash/manual/html_node/Exit-Status.html) and common shell conventions so that `$?` and scripted behavior match bash. **Verified by:** Phase 1 (exit 0/42/255, exit no args); Hardening §10, §11, §14, §17 (exit 256/257, -1, non-numeric, too many args; 127/126; pipeline last command). See [BEHAVIOR.md](BEHAVIOR.md) §11.

### 8.0 Named constants (`includes/defines.h`)

Use these in C instead of bare numbers: **`SUCCESS`** / **`FAILURE`** (0/1 for general outcomes), **`TOK_N`** / **`TOK_Y`** (tokenizer step: character not handled vs handled — check **`OOM`** before treating as boolean), **`XSYN`** (2), **`XNX`** (126), **`XNF`** (127), **`XSB`** (128, signal base), **`XSINT`** (130 = **`XSB + SIGINT`**). For a terminated child, use **`XSB + WTERMSIG(status)`** when **`WIFSIGNALED`**. Bash documentation often names the same numbers with longer **`EXIT_*`** spellings; this repo’s header uses the short forms above.

### 8.1 Summary Table

| Scenario                     | Exit Code      | Macro / expression (`defines.h`)        | Bash reference / usage                          |
| ---------------------------- | -------------- | ---------------------------------------- | ----------------------------------------------- |
| Command success              | `0`            | `SUCCESS`                                | Normal success                                  |
| Command general error        | `1`            | `FAILURE`                                | General failure; builtin "too many args" return |
| Syntax error (shell misuse)  | `2`            | `XSYN`                                   | Misuse of shell builtin / syntax error           |
| Permission denied (exec)     | `126`          | `XNX`                                    | File found but not executable                    |
| Command not found            | `127`          | `XNF`                                    | Command not in PATH                             |
| Fatal signal N              | `128 + N`      | `XSB + N`                                | Child killed by signal N (e.g. 130 = SIGINT)     |
| Ctrl+C (SIGINT)              | `130`          | `XSINT`                                  | Same as **`XSB + SIGINT`**                      |
| Ctrl+\ (SIGQUIT)             | `131`          | `XSB + SIGQUIT`                          | `128 + 3`                                       |
| `exit` with valid arg        | `arg % 256`    | —                                        | 0–255; out-of-range wraps (e.g. 256 → 0)        |
| `exit` with non-numeric      | `255` (bash) / **`2` (this shell)** | `XSYN` (us) | Bash: stderr + exit 255. **We:** same message, **`XSYN`** (known difference). |
| `exit` with too many args    | (no exit)      | returns `FAILURE`                        | Print error to stderr, shell continues |

### 8.2 Where We Use Each Code

```mermaid
flowchart LR
    subgraph set["Who sets last_exit"]
        EX[run_commands / builtins]
        SY[syntax_check / tokenizer errors]
        HD[heredoc SIGINT]
        CS[check_signal_received SIGINT]
        XT[builtin_exit / clean_exit]
    end
    EX --> LE([shell last_exit])
    SY --> LE
    HD --> LE
    CS --> LE
    XT --> LE
    LE --> MAIN[main return value]
```

- **`SUCCESS` (0)** – Successful builtin or external command.
- **`FAILURE` (1)** – Builtin failure (e.g. `exit` too many args **returns** `FAILURE`), redirection failure, or generic error.
- **`XSYN` (2)** – Syntax error (`syntax_check`, tokenizer unclosed quote); also **`exit` with non-numeric arg** (bash uses **255**).
- **`XNX` (126)** – Path is directory or permission denied
  around `execve` (`exec_child.c` **`child_abort_msg`**).
- **`XNF` (127)** – Command not found
  (**`child_exit_not_found`**; pipeline fast path **`pip_all_nf`** in
  parent).
- **`XSB + signal`** – Child terminated by signal; e.g. **`XSINT`** for SIGINT (`exec_external.c` **`child_exit_status`**, pipeline **`wait_pipes`**). **`SIGQUIT`** during a single external wait also prints **`Quit (core dumped)\n`** from **`child_exit_status`**; pipelines use the numeric status only (see [SIGNAL.md](SIGNAL.md)).

### 8.3 exit Builtin (Bash Reference)

- **Builtin `exit` (interactive):** Bash prints `"exit\n"` to **stderr** before exiting. We match with **`ft_dprintf(STDERR_FILENO, "exit\n")`** in **`builtin_exit`** when **`isatty(STDIN_FILENO)`** (`exit.c`).
- **Readline EOF (Ctrl+D):** When **`readline`** returns **`NULL`**, **`read_input`** prints **`exit\n`** via **`ft_printf`** (stdout) then leaves the REPL (`repl_loop.c`) — differs from bash’s stream choice but matches current code.
- **Non-numeric argument:** Bash exits with **255** after printing "numeric argument required" to stderr. **We exit `XSYN` (2)** (same message; exit code differs — see `exit.c`).
- **Too many arguments:** Bash does not exit; it prints an error and returns 1. We match this (**`FAILURE`**).

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
# Behavior (mostly bash; non-numeric uses XSYN / 2 here, bash uses 255):
exit                    # Exit with last command's status
exit 0                  # Exit with 0
exit 42                 # Exit with 42
exit 256                # Exit with 0 (256 % 256)
exit -1                 # Exit with 255 (two's complement)
exit abc                # Error to stderr: "numeric argument required", then XSYN (bash: 255)
exit 1 2 3              # Error to stderr: "too many arguments", return FAILURE / 1, do NOT exit
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
void	reset_shell(t_shell *shell)
{
	if (shell->tokens)
		free_tokens(&shell->tokens);
	shell->tokens = NULL;
	if (shell->cmds)
		free_cmds(&shell->cmds);
	shell->cmds = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
}
```

**Does not** clear **`envp`**, **`user`**, **`cwd`**, or **`last_exit`** (see **§2**).

### 11.2 Exit Cleanup (actual: `free/free_shell.c`)

```c
void	free_all(t_shell *shell)
{
	if (shell->tokens)
		free_tokens(&shell->tokens);
	shell->tokens = NULL;
	if (shell->cmds)
		free_cmds(&shell->cmds);
	shell->cmds = NULL;
	if (shell->envp)
		free_envp(shell->envp);
	shell->envp = NULL;
	if (shell->user)
		free(shell->user);
	shell->user = NULL;
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
}
```

**`main()`** calls **`rl_clear_history()`** before **`free_all()`**; **`builtin_exit`** → **`clean_exit`** → **`free_all`** on shell exit.

### 11.3 Defensive pattern

Call sites **NULL** out pointers after freeing (as in **`reset_shell`** / **`free_all`**) so double-free paths are easier to spot. There is **no** shared **`safe_free`** helper in this repo—the pattern is applied **inline**.

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

## 13. Implementation Order

```
Phase 1: Foundation
├── [x] Shell struct and initialization (`init/init_shell.c`, `init/init_env.c`, structs.h)
├── [x] Main loop with readline (TTY) / ft_read_stdin_line (non-TTY) (`repl_loop.c`, `msh_stdin_read_line.c`)
├── [x] Basic signal handling (signals/signal_handler.c, signal_utils.c)
└── [x] Builtins (echo, cd, pwd, export, unset, env, exit)

Phase 2: Tokenizer & parser
├── [x] Tokenizer (tokenizer/tokenizer.c, tokenizer_ops.c, tokenizer_handlers.c, tokenizer_quotes.c)
├── [x] Quote handling (unclosed quote -> syntax error, no continuation)
├── [x] Syntax validation (parser/parse_syntax.c)
└── [x] Command table construction (parser/parse_input.c, parse_pipeline.c, parse_attach_token.c, parse_redir.c, parse_finalize.c)

Phase 3: Expander
├── [x] Variable expansion (tokenizer/expansion.c, tokenizer/expansion_utils.c — $VAR, $?)
├── [x] Exit status expansion ($?)
├── [x] Quote removal (during tokenization)
└── [x] Edge case handling (e.g. $ at end, invalid names)

Phase 4: Executor (Simple)
├── [x] Single external command execution (executor/exec_external.c)
├── [x] Path resolution (resolve_cmd_path in exec_external.c)
├── [x] Single builtin with redirections (run_single_builtin / run_external)
└── [x] File redirections (executor/exec_redir.c, apply_one_redir, hd_fd)

Phase 5: Pipes & Heredoc
├── [x] Pipeline execution (executor/exec_pipeline.c)
├── [x] Heredoc implementation (parser/heredoc_collect.c, parser/heredoc_io.c, parser/heredoc_expand.c)
├── [x] Multiple redirections (cmd->redirs list, left-to-right)
└── [x] Signal handling in children (wait_pipes, `XSB` + signal)

Phase 6: Polish & Refactor
├── [x] Error messages (minishell: cmd: msg style)
├── [x] Memory cleanup (free/free_shell.c, free/free_runtime.c, reset_shell)
├── [x] Edge case handling (hardening tests pass)
├── [x] Executor split: `exec_child`, `exec_notfound`, `exec_pipe_step`, `exec_pipeline_nf` (`pip_all_nf`)
├── [x] Exit utils extracted: parse_exit_value in exit_utils.c
├── [x] Pipeline **`sync_fd`** currently inactive; stderr ordering for all-not-found via **`pip_all_nf`**
└── [x] Norminette / 42 compliance (`norminette` on `includes`, `libft`, `src` in Linux)
```

---

## 14. Related documentation

| Document | Purpose |
|----------|---------|
| **[BEHAVIOR.md](BEHAVIOR.md)** | Test-backed behavior: redirections, pipes, expansion, builtins, exit codes, path resolution, input resilience. Use for evaluation and debugging. |
| **[DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md)** | **Data model:** why we chose each struct/enum. **Function reference:** every function by file with one-line description; Mermaid call flow. |
| **[`includes/defines.h`](../includes/defines.h)** | Shared macros: **`SUCCESS`/`FAILURE`**, **`TOK_N`/`TOK_Y`**, **`RL_LN`/`RL_EOF`/`RL_SIG`**, **`XSYN`**, **`XNF`/`XNX`**, **`XSB`**, **`XSINT`**, **`OOM`**, **`PR_ERR`**, **`STDIN_LAST_*`**, **`HD_*`**, **`S_EMPTY`/`S_AMBIG`**, prompt pieces (**`PM_*`**). |
| **[SIGNAL.md](SIGNAL.md)** | Signal dispositions, **`g_signum`**, heredoc/pipeline/repl cases. |
| **README.md** | Project overview, build, usage, how to run tests. |

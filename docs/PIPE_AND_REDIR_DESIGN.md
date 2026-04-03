# Pipes & Redirections — Design Notes (for defense)

This document explains **how this minishell implements pipelines and redirections**, why the design is safe/correct, and what is intentionally **out of scope** per the 42 minishell subject.

For the “big picture” architecture diagrams, also see `docs/MINISHELL_ARCHITECTURE.md` (it already contains a full flow from input → tokenize → parse → heredoc → execute).

---

## 1. What we build in the parser (data model)

### 1.1 Pipeline as a list of `t_command`

After tokenization and syntax validation, the parser builds a **pipeline** as a `t_list*` where each node’s `content` is a `t_command*`.

- **Pipe split**: token `PIPE` creates a **new** `t_command` and appends it to the list.
  - Implementation: `src/parser/parse_pipeline.c` (`build_command_list`, `append_pipe_command`).

### 1.2 Redirections as a list of `t_redir`

Each `t_command` owns a `t_list* redirs`. Each node’s `content` is a `t_redir*`:

- `t_redir.file`: target filename (already expanded; may be marked ambiguous)
- `t_redir.fd`: destination fd (`STDIN_FILENO` or `STDOUT_FILENO` in our current scope)
- `t_redir.append`: `0` for truncating output (`>`), `1` for append (`>>`)

Parser responsibilities:

- **Syntax**: ensure redirection tokens are followed by a `WORD`
  - `src/parser/parse_syntax.c` (`syntax_check`, `check_redir_syntax`)
- **Build redir nodes** for `<`, `>`, `>>`
  - `src/parser/parse_redir.c` (`parse_redir_token_pair`, `append_redir`)

### 1.3 Heredoc: delimiter stored, then fd produced

The parser stores the **latest** heredoc delimiter seen in a command:

- `cmd->hd_delim` (string)
- `cmd->hd_quoted` (controls whether `$VAR` expands inside heredoc)

Later, heredoc input is read into a pipe, and the **read end** is stored as:

- `cmd->hd_fd`

Implementation:

- Delim capture: `src/parser/parse_attach_token.c` (`handle_heredoc_token`)
- Read loop: `src/parser/heredoc_collect.c` (`read_heredoc`)

---

## 2. What we execute (single command vs pipeline)

Execution dispatch is centralized in `src/executor/exec_dispatch.c` (`run_commands`):

- **Single command (no `|`)**
  - empty argv: apply redirs (if any) then restore stdio
  - builtin: may run in parent *or* in child depending on type + redirs
  - external: fork/exec + waitpid
- **Pipeline (`a | b | c`)**
  - fork one process per segment, wire pipes, apply per-segment redirs, then wait for all children and return the **last segment’s** status

---

## 3. Redirection semantics (open + dup2 + close)

### 3.1 `apply_redirs(t_command*)`

`src/executor/exec_redir.c` applies redirections in list order:

- **Input `< file`**:
  - `open(file, O_RDONLY)`
  - `dup2(fd, STDIN_FILENO)`
  - `close(fd)`
- **Output `> file` / `>> file`**:
  - `open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)` or `O_APPEND`
  - `dup2(fd, r->fd)`
  - `close(fd)`
- **Ambiguous redirect**:
  - if `r->file` has the internal ambiguous prefix `S_AMBIG`, print `ambiguous redirect` and fail

### 3.2 Heredoc vs `< file` (bash left-to-right)

Parse records **`cmd->stdin_last`** (`STDIN_LAST_HD` or `STDIN_LAST_FILE`) from
token order: each `<<` sets heredoc-last; each `< file` sets file-last.

After applying all **`cmd->redirs`** left-to-right:

- if `cmd->hd_fd != -1` and **`stdin_last == STDIN_LAST_HD`**, `dup2` the
  heredoc read end to stdin (then close it and set `-1`)
- if `cmd->hd_fd != -1` and **`stdin_last == STDIN_LAST_FILE`**, only **close**
  the heredoc read fd (stdin already comes from the last `<`)

So whichever of `<` and `<<` appears **last in the source** wins stdin, like bash.

---

## 4. Pipeline wiring (FD ownership rules)

### 4.1 Per-segment wiring

Pipeline execution is in:

- `src/executor/exec_pipeline.c` (`run_pip`,
  `wait_pipes`)
- `src/executor/exec_pipe_step.c` (`pipe_step`)

For each segment:

- if there is a previous pipe read-end (`prev_fd != -1`), the child does:
  - `dup2(prev_fd, STDIN_FILENO)` then closes `prev_fd`
- if there is a next segment, the child does:
  - `dup2(pipe_write, STDOUT_FILENO)` then closes the original pipe fds

**Important correctness property:** every `dup2()` is followed by closing the old fd, preventing leaks and keeping the pipe reference counts correct (so EOF propagates and you don’t deadlock waiting on a pipe that is still open in some process).

### 4.2 Parent closes what it no longer owns

In `pipe_step`:

- parent closes the previous `prev_fd` after forking the child (it is no longer needed in the parent)
- parent keeps only the **read end** of the new pipe as the next `prev_fd`

This ensures the parent doesn’t accidentally keep pipe ends open (classic bug that prevents the last reader from seeing EOF).

---

## 5. Child lifecycle: do we “kill” children? do we reap them?

### 5.1 Reaping is mandatory (no zombies)

We **reap** children via `waitpid`:

- **Single external**: `src/executor/exec_external.c` (`run_external`) forks once and `waitpid(pid, ...)` for that pid.
  - We also handle `EINTR` by retrying `waitpid` (theoretical correctness).
- **Pipeline**: `src/executor/exec_pipeline.c`
  (`wait_pipes`) loops until it has reaped **N** children.
  - It calls `waitpid(-1, ...)` and on `EINTR` it retries without counting progress.

This means: the parent shell does not leave unreaped children behind (no zombies).

### 5.2 “Killing” is not the normal mechanism

In a foreground shell, children terminate normally (exit) or due to signals (SIGINT, SIGQUIT, …).

Our signal strategy:

- parent ignores SIGINT/SIGQUIT while waiting (so the shell stays alive)
- children restore default dispositions so they can receive SIGINT/SIGQUIT like in bash

Implementation: `src/signals/signal_handler.c` (`set_signals_ignore`, `set_signals_default`, `set_signals_interactive`).

---

## 6. Builtins and redirections (why some run in parent vs child)

Stateful builtins must run in the **parent process** to affect the shell:

- `cd` changes cwd
- `export`/`unset` mutate the environment
- `exit` terminates the shell

In `src/executor/exec_dispatch.c` (`run_single_builtin`), we run these in parent and
apply/restore redirections by backing up stdio (`dup`) and restoring (`dup2`).

For other builtins, if the command has redirections, we may run them via the **external path** (`fork`) to avoid parent stdio side-effects.

Design constraint: this keeps the parent’s REPL stable and reduces “stdio got stuck redirected” bugs.

---

## 7. What we intentionally do NOT implement (scope)

### 7.1 Control operators / job control (out of mandatory scope)

We do not implement (as required features):

- `&&`, `||`, `;` command chaining
- background jobs `&`, job control (`fg`, `bg`, `jobs`)
- subshell/grouping `(...)`, `{ ...; }`

### 7.2 Advanced fd redirection forms (not in our current parser/executor)

Current redirection parsing maps tokens to only:

- `< file` → stdin
- `> file` / `>> file` → stdout
- `<< delim` → heredoc pipe; whether it becomes stdin depends on **`stdin_last`**
  vs any `< file` on the same command (last in source wins)

We do **not** currently parse/execute forms like:

- `2> file`, `2>> file` (redirect stderr)
- `n> file` for arbitrary fd `n`
- `>&`, `<&`, `2>&1`, fd duplication/closing operators

If you need these, it requires extending tokenization/parsing to carry the numeric fd and the redirection operator semantics.

---

## 8. Constraints and trade-offs in this design

- **Child-process Valgrind cleanliness**: with `valgrind --trace-children=yes`, external programs (e.g. `/usr/bin/ls`) may show “still reachable” allocations. That is **owned by the child program**, not by minishell. Minishell’s responsibility is to close its own fds, free its own heap, and reap the children.
- **Heredoc delimiter handling**: this design stores the latest heredoc delimiter per command; multiple heredocs on one command follow that policy (document this if you rely on it).
- **Internal sentinel for ambiguous redirects**: expansion marks ambiguous redirects by prefixing filenames with `S_AMBIG`; executor checks this prefix. This is simple and keeps ambiguity policy centralized.
- **No job control**: without process groups + tcsetpgrp, signal delivery is “good enough” for the mandatory scope but not a full interactive shell.

---

## 9. Quick mental model (defense-friendly)

1. **Parser builds structure**: pipeline list of commands; each command has argv parts + redir list + heredoc metadata.
2. **Executor chooses path**:
   - single command: parent builtin (stateful) or fork/exec (external or “child builtin”)
   - pipeline: fork N children, wire pipes, apply per-child redirs, exec/builtin in child
3. **FD safety rules**:
   - every `dup2` is followed by closing the original fd
   - parent closes unused pipe ends immediately
4. **Process safety rules**:
   - parent always `waitpid`/reaps children
   - parent ignores interactive SIGINT/SIGQUIT during wait, children default

---

## 10. Executor naming map (readability refactor)

Some executor-internal static helpers were renamed to be clearer for teammates.
Behavior is unchanged.

- `bk_fd` -> `backup_stdio_fds` (`src/executor/exec_dispatch.c`)
- `rs_fd` -> `restore_stdio_fds` (`src/executor/exec_dispatch.c`)
- `run_empty` -> `run_empty_command` (`src/executor/exec_dispatch.c`)
- `run_bi` -> `run_single_builtin` (`src/executor/exec_dispatch.c`)
- `ch_fds` -> `setup_pip_child_fds` (`src/executor/exec_pipe_step.c`)
- `fork_pl` -> `fork_pip_child` (`src/executor/exec_pipe_step.c`)
- `adv_prev` -> `advance_prev_pipe_fd` (`src/executor/exec_pipe_step.c`)
- `wait_one` -> `upd_wait_st`
  (`src/executor/exec_pipeline.c`)
- `wait_nlast` -> `wait_pipes`
  (`src/executor/exec_pipeline.c`)
- `pl_loop` -> `spawn_pipes` (`src/executor/exec_pipeline.c`)
- `need_dq` -> `needs_dollar_quotes` (`src/executor/exec_notfound.c`)
- `esc_c` -> `append_escaped_char` (`src/executor/exec_notfound.c`)
- `fill_dq` -> `fill_dollar_quoted_name` (`src/executor/exec_notfound.c`)
- `fmt_nf` -> `format_not_found_name` (`src/executor/exec_notfound.c`)
- `rdr_in` -> `apply_input_redir` (`src/executor/exec_redir.c`)
- `rdr_out` -> `apply_output_redir` (`src/executor/exec_redir.c`)
- `rdr_one` -> `apply_one_redir` (`src/executor/exec_redir.c`)
- `is_nf_cmd` -> `is_simple_not_found_command`
  (`src/executor/exec_pipeline_nf.c`)
- `bi_child` -> `run_builtin_in_child` (`src/executor/exec_child.c`)
- `ch_abort` -> `child_abort_msg` (`src/executor/exec_child.c`)
- `ch_nf` -> `child_exit_not_found` (`src/executor/exec_child.c`)
- `ch_stat` -> `child_wait_st` (`src/executor/exec_external.c`)
- `mk_path` -> `path_cand` (`src/executor/exec_external.c`)
- `path_scan` -> `scan_path` (`src/executor/exec_external.c`)


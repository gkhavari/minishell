# Minishell Expected Behavior & Test-Design Guide

This document defines **expected behavior** for minishell: what output and exit code the shell must produce for given inputs. It serves as (1) a **behavior specification** for the program and (2) a **guide for designing tests** for those cases.


**Subject vs bash:** Where the PDF says you **must not** interpret **`;`** as a separator or treat **`\`** as a *required* metacharacter, behavior may differ from bash; the tables below still describe **bash** for test design, with **explicit deltas** for this repo. The tokenizer still applies **`\`** in **`ST_NORMAL`** (e.g. before **`$`**)—see [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) §3.

**Exit-status macros (code):** **`includes/defines.h`** uses short names: **`SUCCESS`** / **`FAILURE`** (0/1 for general function outcomes), **`XSYN`** (2), **`XNX`** (126), **`XNF`** (127), **`XSB`** (128, signal base), **`XSINT`** (130 = SIGINT). Signal exits use **`XSB + WTERMSIG(status)`** when **`WIFSIGNALED`**. **Readline path:** **`RL_LN`**, **`RL_EOF`**, **`RL_SIG`**. **`OOM`** is an internal sentinel (e.g. **`build_prompt`** or tokenizer/parser allocations), not a shell exit code. Bash docs often use longer **`EXIT_*`** names for the same numbers; see [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) §8.


## 0. Map to [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md)

| What you need | Where in the architecture doc |
|---------------|-------------------------------|
| Tester layout, script map (`cmds/mand/`) | §0.1, §0.2 |
| **readline** vs **`ft_read_stdin_line`**, empty-line gate (**`input[0]`**), non-TTY syntax **break**, **`main` return `last_exit`** | §2 |
| **`g_signum`**, **`check_signal_received`**, **`rl_event_hook`**, SIGPIPE install | §1, [SIGNAL.md](SIGNAL.md) |
| Tokenizer loop, operators, heredoc delimiter mode | §3 (e.g. §3.2.1) |
| Expansion vs heredoc body, **`~`** | §4 |
| **`parse_input`**: **`syntax_check`** then **`build_command_list`** → **`finalize_cmds`** | §5 |
| Heredoc pipes, SIGINT during heredoc → **`last_exit = XSINT`** (130), no **`run_commands`** | §6 |
| **`run_commands`**, single vs pipeline, parent-only builtins (cd/export/unset/exit), not-found fast path | §7 |
| Exit codes, who sets **`last_exit`**, **`reset_shell` does not clear `last_exit`** | §8, §11 |

For **structs and every function by file**, see [DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md).

### 0.2 Mandatory builtins vs [minishellv10.md](minishellv10.md) (Chapter IV)

Subject **v10.0** requires these builtins only (no `export`/`unset` flags like **`-p`** beyond what bash-style scripts need; **`echo`** only **`-n`** as a special case):

| Subject builtin | Implementation | Notes / validation |
|-----------------|----------------|-------------------|
| **`echo`** with **`-n`** | `builtins/echo.c` | Multiple **`-n`** / **`-nn`** like bash; tested **`1_builtins_echo.sh`**. |
| **`cd`** with a path | `builtins/cd.c` | No-arg → **`$HOME`**; **`-`** → **`$OLDPWD`** (bash-like); optional leading **`--`** then operand (**`cd -- /tmp`**) — POSIX; two separate operands (e.g. **`cd /a /b`**) → **too many arguments** (bash). |
| **`pwd`** with no options | `builtins/pwd.c` | Extra words are ignored (bash-like for this tester); prints **`shell->cwd`**. |
| **`export`** with no options | `builtins/export.c`, `export_utils.c`, `export_print.c` | No **`export -x`**; bare **`export`**, **`KEY=value`**, **`KEY+=value`**; invalid id → stderr + exit **1**. **`1_builtins_export.sh`**. |
| **`unset`** with no options | `builtins/unset.c` | Leading **`-`** on a name → invalid option / syntax-style error; **`1_builtins_unset.sh`**. |
| **`env`** with no args | `builtins/env.c` + **`parse_finalize.c`** | **`env` alone** → builtin lists env. **Any extra token** → **`is_builtin = 0`** → run **external** **`env`** from **`PATH`** (subject: not the zero-arg builtin case). **`1_builtins_env.sh`**. |
| **`exit`** with no options | `builtins/exit.c`, `exit_utils.c` | Numeric arg, too many args, non-numeric; **delta:** non-numeric exits with **`XSYN` (2)**, not bash **255** — see §4. **`1_builtins_exit.sh`**. |

**Dispatcher:** `builtins/builtin_dispatcher.c` (**`run_builtin`**, **`get_builtin_type`**).

---

### 0.1 Design rationale

Why behavior matches this shape:

- **Tokenizer-first design:** quote and expansion policy is decided while
  tokenizing, so parser logic stays simple and deterministic.
- **Syntax gate before build:** `syntax_check` rejects invalid token
  structure early (pipe adjacency, redir without WORD), preventing partial
  command graphs.
- **Per-command execution model:** parser emits one `t_command` per pipeline
  stage; executor can apply redirs and builtin rules stage-by-stage.
- **Last-status rule in pipelines:** parent tracks last child PID and reports
  that status, matching bash pipeline exit semantics.
- **Parent-only stateful builtins:** `cd`, `export`, `unset`, `exit` stay in
  parent for single commands so shell state changes persist.
- **Subject scope discipline:** unsupported grammar (`&&`, `||`, `;`,
  advanced FD operators) is intentionally excluded to keep mandatory behavior
  stable and testable.


## 1. Echo (`1_builtins_echo.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `echo hello` | `hello` + newline | none | 0 |
| `echo hello world` | `hello world` + newline | none | 0 |
| `echo -n hello` | `hello` (no newline) | none | 0 |
| `echo -n -n hello`, `echo -nnnnn hello` | `hello` (no newline) | none | 0 |
| `echo` | newline only | none | 0 |
| `echo ""`, `echo ''` | newline only | none | 0 |
| `echo -n` | nothing | none | 0 |
| `echo -n -a hello` | `-a hello` + newline (first non-flag arg stops `-n` parsing) | none | 0 |
| `echo 'hello world'`, `echo "hello world"` | `hello world` + newline | none | 0 |
| `echo "$PWD"` | value of `PWD` + newline | none | 0 |
| `echo '$PWD'` | literal `$PWD` + newline | none | 0 |
| `echo $?` | last exit status (e.g. `0` or `1`) + newline | none | 0 |
| `echo \$USER` | literal `$USER` + newline (backslash escapes `$`) | none | 0 |
| `echo \\$USER`, `echo \\\$USER`, etc. | As in bash (backslash and expansion rules) | none | 0 |

**Test-design note:** Add cases for empty args, multiple `-n`, `-n` with non-flag arg, single/double quotes, `$VAR` and `$?`, and backslash before `$`. See `1_builtins_echo.sh` and `0_compare_parsing.sh` for many examples.

---

## 2. PWD & CD (`1_builtins_pwd.sh`, `1_builtins_cd.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `pwd` | Current working directory path + newline | none | 0 |
| `pwd oi` (extra args) | Same as `pwd` (bash ignores extra args) | none | 0 |
| `cd /tmp` then `pwd` | (second line) `/tmp` or equivalent | none | 0 |
| `cd` (no args) | — | none | 0; changes to `$HOME` |
| `cd -` | New cwd path (after chdir to OLDPWD) | none | 0 |
| `cd $PWD` | — | none | 0 |
| `cd /tmp /var`, `cd $PWD hi` (two path operands) | — | `cd: too many arguments` (or equivalent) | **1** (bash; this repo **FAILURE**) |
| `cd -- /tmp` | — | none | **0**; operand is **`/tmp`** (leading **`--`** ends option scan, POSIX) |
| `cd --` (no further operand) | — | none | **0**; same as **`cd`** → **`$HOME`** when **`HOME`** set |
| `cd 123123` (non-existent) | — | message e.g. "No such file or directory" | 1 |
| `cd '/////'` (normalized by bash to `/`) | — | bash: none; minishell may differ (path normalization) | bash 0 |

**Test-design note:** Cover no-arg (HOME), `-` (OLDPWD), **`cd -- <path>`**, two operands (error), nonexistent path (exit 1). See `1_builtins_pwd.sh`, `1_builtins_cd.sh`.

---

## 3. Env, Export, Unset (`1_builtins_env.sh`, `1_builtins_export.sh`, `1_builtins_unset.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `env` | List of `VAR=value` lines (environment) | none | 0 |
| `env` **with arguments** (e.g. `env what`, `env -i ./minishell`) | — | As bash: **not** the builtin-with-no-args case — run the **external** `env` from PATH (or absolute path) like a normal command | per command |
| `export` | Exported variables (e.g. `declare -x` form in bash) | none | 0 |
| `export HELLO=123` then `env` | Second line shows `HELLO=123` (or equivalent) | none | 0 |
| `export 1INVALID=x`, `export 1BADNAME=x` | — | "not a valid identifier" or similar | 1 |
| `export A-=x`, `export =` | — | error | 1 |
| `unset VAR` then `env` | `VAR` no longer in env | none | 0 |
| `unset`, `unset ""` | — | error or no-op per bash | 0 or 1 |

**Test-design note:** Valid names (letters, digits, underscore); invalid (leading digit, `-`, `=`) must produce stderr and exit 1. Subject: builtin **`env`** is **no options or arguments** — any `env` with args is executed as **external** `env` (this repo: `parse_finalize` / dispatch). See the three `1_builtins_*` scripts above.


---

## 4. Exit (`1_builtins_exit.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `exit` | — | none (interactive: "exit" to stderr) | **last command’s exit status** |
| `exit 0` | — | none | **0** (shell exits) |
| `exit 42`, `exit 255` | — | none | **42**, **255** (shell exits) |
| `exit 256` | — | none | **0** (256 mod 256; shell exits) |
| `exit 257` | — | none | **1** (257 mod 256; shell exits) |
| `exit -1` | — | none | **255** (unsigned 8-bit; shell exits) |
| `exit 00042` | — | none | **42** (leading zeros; shell exits) |
| `exit +100`, `exit "+100"` | — | none | **100** (shell exits) |
| `exit hello` (non-numeric) | — | "numeric argument required" or similar | **255** (bash; **42 tester** compares exit code) |
| `exit 42 world` (too many args) | — | "too many arguments" or similar | **1** (shell does **not** exit) |
| `exit 1 2`, `exit 1 2 3` | — | "too many arguments" | **1** (shell does not exit) |

**Implementation note (this repo):** For non-numeric `exit`, **`exit.c` exits with `XSYN` (2)**, not bash’s **255**. Mandatory tester will disagree with bash on **EXIT_CODE** for those blocks until the code matches bash.

**Test-design note:** Numeric: exit with `n % 256`. Non-numeric (bash): stderr + exit **255**. Too many args: stderr + **`FAILURE` (1)**, **no exit**. See `1_builtins_exit.sh`.

---

## 5. Variable Expansion (`0_compare_parsing.sh`, `1_builtins_echo.sh`, `1_variables.sh`, `11_expansion.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `echo $UNSET_VAR` | empty line | none | 0 |
| `echo $` | `$` | none | 0 |
| `echo $?` after `true` | `0` | none | 0 |
| `echo $?` after `false` | `1` | none | 0 |
| `echo '$HOME'` | literal `$HOME` | none | 0 |
| `echo "hello $VAR"` (VAR=world) | `hello world` | none | 0 |
| `export A_B=1` then `echo $A_B` | `1` | none | 0 |

**Test-design note:** Single quotes: no expansion. Double quotes: expand `$VAR`, `$?`. Unset → empty. See `0_compare_parsing.sh`, `1_builtins_echo.sh`, `1_variables.sh`, `11_expansion.sh`.

---

## 6. Redirections (`1_redirs.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `echo hello > file` then `cat file` | `hello` | none | 0 |
| `echo line1 > file` then `echo line2 >> file` then `cat file` | `line1` then `line2` | none | 0 |
| `cat < file` (file exists) | contents of file | none | 0 |
| `cat < /no_such_file` | — | "No such file" or similar | 1 |
| `echo hi > /no_such/dir/file` | — | error (directory or path) | 1 |
| `echo a > f1 > f2` | — | none | 0; only **last** file gets output |
| `echo err 2> file` (stderr to file) | `2` then newline to stdout; nothing redirected to file | none | 0; **`2>` is not implemented** — tokenizer produces WORD `2` + REDIR_OUT, so `2` is an argument and `>` redirects stdout |
| Redirect to directory (e.g. `echo x > /tmp`) | — | error | 1 |

**Test-design note:** Multiple `>`: last wins. Missing input file: stderr + exit 1. **`2>` stderr redirect is NOT implemented** — the tokenizer does not recognize `2>` as a compound operator; the `2` becomes a WORD argument. The `t_redir.fd` field supports arbitrary FD targets, but the tokenizer/parser don’t produce `STDERR_FILENO` redirections. See `1_redirs.sh`.

---

## 7. Heredocs (`1_redirs.sh`, `1_pipelines.sh`, `10_parsing_hell.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `cat << EOF` then `hello` then `EOF` | `hello` | none | 0 |
| `cat << EOF` then `hello $VAR` then `EOF` (VAR set) | `hello <value>` | none | 0 |
| `cat << 'EOF'` then `hello $HOME` then `EOF` | literal `hello $HOME` | none | 0 |
| Delimiter quoted or with trailing text: `<< 'lim'`, `<< lim''` | As in bash (delimiter recognition) | none | 0 |
| `cat < file <<EOF` … `EOF` vs `cat <<EOF` … `EOF` `< file` (same line layout as bash) | **bash:** stdin is whichever of `< file` and `<<` appears **last** in the command’s source order (left-to-right). | none | 0 |

**Test-design note:** Quoted delimiter → no expansion in body. Unquoted → expand variables. See `1_redirs.sh`, `10_parsing_hell.sh`.

**SIGINT during heredoc:** Parent reads the heredoc body; if **`g_signum == SIGINT`** after a line, **`process_heredocs`** fails, **`process_input`** sets **`last_exit = XSINT`**, and **does not** run **`run_commands`** (**[MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) §6**). Matches bash-style interrupt handling for the “open heredoc” case in tests.

---

## 8. Pipes (`1_pipelines.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `echo hello \| cat` | `hello` | none | 0 |
| `echo a \| cat \| cat` | `a` | none | 0 |
| `true \| false` | (none) | none | **1** (last command) |
| `false \| true` | (none) | none | **0** (last command) |
| `cd /tmp \| pwd` | (pwd output) | none | 0; **cd in pipe does not change parent** |
| Pipeline with heredoc (e.g. `ls \| cat << E \| grep x`) | As in bash | May match bash on **stderr presence** (see below) | 0 or last command’s status |
| Huge pipeline / `fork` failure | As bash | **Stderr:** e.g. `fork: Resource temporarily unavailable` (or `perror`-style) — presence should match bash | per bash |

**Test-design note:** Pipeline exit = **last** command’s exit code. Builtins in the middle (e.g. `cd`) run in subshell; parent env unchanged.

**SIGPIPE / Linux CI:** On many Linux runners (e.g. systemd), **SIGPIPE is ignored** in the parent. **Bash** does not reset it to default for pipeline children; they **inherit SIG_IGN**. Then **GNU** tools such as `ls` / `yes` may write **“write error: Broken pipe”** (or similar) to **stderr** when the pipe breaks. This shell **inherits the same rule** so mandatory **stderr presence** matches bash on Ubuntu. On macOS, the parent often has SIGPIPE **default**, so local stderr may differ from CI — **trust Linux** for tester alignment. See **[MINISHELL_ARCHITECTURE.md §1](MINISHELL_ARCHITECTURE.md)** (signal install).

**Current local residual (Linux):** one stubborn long-pipeline case (`1_pipelines.sh:180`) can differ only by ordering of repeated `command not found` lines across many concurrent children.

**Scheduling hardening note (2026-03-28 → superseded 2026-03-30):** A launch
barrier (`sync_fd` pipe) was explored but is currently **inactive** (`sync_fd[0] = -1`,
`sync_fd[1] = -1` in `run_pip`). The all-not-found fast path
(`pip_all_nf` in `exec_pipeline_nf.c`) handles the ordering problem for that case
by printing errors in the parent before forking — no barrier needed.
Children still read from `pipe_fd[2]` only if `sync_fd[0] != -1`.

**Current m-only unstable case:** `2_correction.sh:221` can differ because `mkdir/chmod/cd/rm` side effects in the shared tmp-dir run order produce a bash/minishell stderr delta even when core behavior is otherwise aligned.

See `1_pipelines.sh`.

---

## 9. Syntax Errors (`8_syntax_errors.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `\|` (lone pipe) | — | "syntax error" (e.g. "unexpected token `\|`") | **2** |
| `\| echo hi` | — | syntax error | **2** |
| `echo hi \|` (pipe at end) | — | syntax error (e.g. "unexpected token `\|`" or "end of file") | **2** |
| `echo hi >` (redirect, no file) | — | syntax error (e.g. "newline") | **2** |
| `>`, `>>`, `<<` (alone) | — | syntax error | **2** |
| `>> >> >>` | — | syntax error | **2** |
| `cat <<` (no delimiter) | — | syntax error | **2** |
| `>| echo wtf` (clobber; bash accepts `>|`) | — | bash: none, exit 0; minishell may treat as syntax error, exit **2** | 0 (bash) / 2 (minishell if not implemented) |

**Test-design note:** Any invalid token sequence (leading pipe, trailing pipe, redirect without file, heredoc without delimiter) → stderr + exit **2**. See `8_syntax_errors.sh`.

---

## 10. Path & Command Execution (`1_scmds.sh`, `2_path_check.sh`, `9_go_wild.sh`)

| Input | Expected stdout | Expected stderr | Exit |
|-------|-----------------|-----------------|------|
| `/bin/echo hello` | `hello` | none | 0 |
| `nonexistent_cmd_xyz` | — | "command not found" or similar | **127** |
| `/tmp` (directory as command) | — | "Is a directory" or similar | **126** |
| Empty or minimal PATH (e.g. `env -i ./minishell` then `ls`) | — | "command not found" | **127** |
| **`PATH` unset** but shell started with PATH present (`had_path`) | Match bash for default search list | Often includes **`.`**; non-executable file in cwd → **126** (not 127) when bash does | per bash |

**Test-design note:** Absolute path: exec if executable. Name in PATH: search then exec; not found → 127. Directory as command → 126. Resolution uses startup **`had_path`** and fallback paths — see **§7.7** in [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md). See `1_scmds.sh`, `2_path_check.sh`, `9_go_wild.sh`.

---

## 11. Exit Code Summary (Bash-Aligned)

**Process exit:** **`main()`** returns **`shell.last_exit`** (bash-like “last foreground status”). **Ctrl+C** at the interactive prompt sets **`XSINT`** (**130**) via **`check_signal_received`** before the next prompt (**[MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) §1–§2, §8.2**, [SIGNAL.md](SIGNAL.md)).

| Code | Macro(s) in `defines.h` | Meaning | Example |
|------|-------------------------|--------|--------|
| 0 | `SUCCESS` | Success | Successful command, `exit 0`, `exit 256` |
| 1 | `FAILURE` | General / builtin error | `cd` to bad dir, `export` invalid name, `exit 1 2` (too many args), pipeline last command failed |
| 2 | `XSYN` | Syntax error | Lone pipe, pipe at end, redirect with no file |
| 126 | `XNX` | Not executable | Running a directory as command |
| 127 | `XNF` | Command not found | Unknown command name |
| 128+N | `XSB + N` | Killed by signal N | e.g. **130** = `XSINT` / SIGINT, **131** = SIGQUIT |
| 255 | — (bash only) | Bash: non-numeric `exit` | Bash exits 255 after "numeric argument required" (**this minishell: `XSYN`** — see §4) |

---

## 12. Not Implemented (Out of Mandatory Scope)

| Feature | Status |
|---------|--------|
| `&&` / `\|\|` | ❌ Not implemented (optional **bonus** in subject — separate from mandatory) |
| `;` (command separator) | ❌ Not implemented |
| Full bash backslash-escape grammar | ❌ Not required — subject: do **not** interpret `\` as a *mandatory* metacharacter. This shell still handles **`\`** in **`ST_NORMAL`** for some cases (e.g. before **`$`**) — see [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md) §3. |

Use multiple input lines to chain commands. The 42 subject does not require `;` or logical **`&&`/`||`** for the **mandatory** part.

---

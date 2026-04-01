# Minishell Expected Behavior & Test-Design Guide

This document defines **expected behavior** for minishell: what output and exit code the shell must produce for given inputs. It serves as (1) a **behavior specification** for the program and (2) a **guide for designing tests** for those cases.

**Primary harness:** **[LeaYeh/42_minishell_tester](https://github.com/LeaYeh/42_minishell_tester)** — same input as **bash**, compare stdout (`diff -q`), normalized stderr (`diff -q` after tester filters), and exit code. Mandatory mode: `tester.sh m` (see `scripts/run_minishell_tester.sh`).

**Run mandatory tests (Docker dev container, from host):**

```bash
./scripts/run_minishell_tester.sh m       # mandatory
./scripts/run_minishell_tester.sh vm      # mandatory + valgrind
```

Build runs inside the container (`make re` / `make debug`). CI clones the same repo (see `.github/workflows/test.yaml`).

**CI / baseline:** Use GitHub Actions workflow logs and local **`mstest_output_*`** directories after **`./scripts/run_minishell_tester.sh`**.

**Subject vs bash:** Where the PDF says you **must not** interpret **`;`** or **`\`**, behavior may differ from bash; the tables below still describe **bash** for test design, with **explicit deltas** for this repo.

For architecture and data structures, see [MINISHELL_ARCHITECTURE.md](MINISHELL_ARCHITECTURE.md).

---

## How the 42_minishell_tester Works

- Reads test **blocks** from scripts under `cmds/mand/` (LeaYeh splits builtins into `1_builtins_echo.sh`, `1_builtins_cd.sh`, …; plus `8_syntax_errors.sh`, `1_redirs.sh`, etc.). Each block is one or more lines of input; empty lines and `#` comments separate blocks.
- For each block: sends the same input to **minishell** and to **bash** (with `enable -n .` prepended so bash runs the test as the last command).
- **Pass** requires: stdout identical (`diff -q`), normalized stderr identical (`diff -q`), and exit code identical.
- **Non-interactive input:** the tester feeds lines on stdin; this shell uses **`read_line_stdin()`** (not readline) for non-TTY stdin — one logical line per tester block.
- The tester runs minishell first, then bash, in the same temporary test directory for each block. Blocks with filesystem side effects can expose order-sensitive differences.

So “expected behavior” here is **bash-like** unless the **subject** or **documented deltas** say otherwise. The tables summarize bash-oriented expectations and point to the 42 script(s) that cover them.

---

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
| `cd $PWD hi`, `cd /tmp /var` | — | none | 0; **only first argument** used as target, rest ignored (bash behavior) |
| `cd 123123` (non-existent) | — | message e.g. "No such file or directory" | 1 |
| `cd '/////'` (normalized by bash to `/`) | — | bash: none; minishell may differ (path normalization) | bash 0 |

**Test-design note:** Cover no-arg (HOME), `-` (OLDPWD), nonexistent path (exit 1), and **extra arguments** (use first only, exit 0). See `1_builtins_pwd.sh`, `1_builtins_cd.sh`.

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

**Test-design note:** Valid names (letters, digits, underscore); invalid (leading digit, `-`, `=`) must produce stderr and exit 1. Subject: builtin **`env`** is **no options or arguments** — any `env` with args is executed as **external** `env` (this repo: `argv_build` / dispatch). See the three `1_builtins_*` scripts above.

**Tester note:** Any block with **`unset TES;T`** (semicolon) depends on **`;`** separator behavior (out of mandatory scope). To test `unset` itself, use **`unset TES`** without semicolon.

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

**Implementation note (this repo):** For non-numeric `exit`, **`exit.c` exits with 2**, not bash’s **255**. Mandatory tester will disagree with bash on **EXIT_CODE** for those blocks until the code matches bash.

**Test-design note:** Numeric: exit with `n % 256`. Non-numeric (bash): stderr + exit **255**. Too many args: stderr + exit 1, **no exit**. See `1_builtins_exit.sh`.

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

**Test-design note:** Quoted delimiter → no expansion in body. Unquoted → expand variables. See `1_redirs.sh`, `10_parsing_hell.sh`.

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
`sync_fd[1] = -1` in `execute_pipeline`). The all-not-found fast path
(`executor_pipeline_not_found.c`) handles the ordering problem for that case
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

| Code | Meaning | Example |
|------|---------|--------|
| 0 | Success | Successful command, `exit 0`, `exit 256` |
| 1 | General / builtin error | `cd` to bad dir, `export` invalid name, `exit 1 2` (too many args), pipeline last command failed |
| 2 | Syntax error | Lone pipe, pipe at end, redirect with no file |
| 126 | Not executable | Running a directory as command |
| 127 | Command not found | Unknown command name |
| 128+N | Killed by signal N | e.g. 130 = SIGINT, 131 = SIGQUIT |
| 255 | Bash: non-numeric `exit` | Bash exits 255 after "numeric argument required" (**this minishell: 2** — see §4) |

---

## 12. Not Implemented (Out of Mandatory Scope)

| Feature | Status |
|---------|--------|
| `&&` / `\|\|` | ❌ Not implemented (optional **bonus** in subject — separate from mandatory) |
| `;` (command separator) | ❌ Not implemented |
| `\` (backslash) as a special escape outside quotes | ❌ Not required — subject: do **not** interpret `\` as a required metacharacter |

Use multiple input lines to chain commands. The 42 subject does not require `;`, `\`, or logical **`&&`/`||`** for the **mandatory** part.

---

## 13. 42_minishell_tester Script Reference (LeaYeh `cmds/mand/`)

| Script | Content |
|--------|---------|
| `0_compare_parsing.sh` | Echo, quotes, expansion (compare to bash parsing) |
| `1_builtins_echo.sh` | `echo`, `-n`, quotes, `$?`, backslash |
| `1_builtins_pwd.sh` | `pwd` |
| `1_builtins_cd.sh` | `cd` |
| `1_builtins_env.sh` | `env` |
| `1_builtins_export.sh` | `export` |
| `1_builtins_unset.sh` | `unset` |
| `1_builtins_exit.sh` | `exit` |
| `1_pipelines.sh` | Pipes, heredocs in pipes |
| `1_redirs.sh` | Redirections, heredocs, combined |
| `1_scmds.sh` | Simple commands, path |
| `1_variables.sh` | Variable expansion cases |
| `11_expansion.sh` | Additional expansion cases |
| `2_correction.sh` | Correction / edge cases |
| `2_path_check.sh` | PATH, env -i |
| `8_syntax_errors.sh` | Syntax error cases (pipe, redirect, etc.) |
| `9_go_wild.sh` | Wild / env -i, PATH |
| `10_parsing_hell.sh` | Parsing edge cases, heredoc, expansion |

Use these scripts as the **source of truth** for designing new tests: add blocks in the same format (one or more lines, blank line or `#` to separate). Expected behavior is **match bash** on stdout, stderr presence, and exit code; **except** where this doc and [minishellv10.md](minishellv10.md) define **subject-only** behavior.

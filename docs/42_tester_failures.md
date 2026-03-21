# 42_minishell_tester — Working notes & status

**Purpose:** Living document for **what was fixed**, **what still fails**, and **how to keep debugging**. Update this file when you land a fix or confirm new tester numbers.

**Run the mandatory tester from repo root:**

```bash
make
./scripts/run_minishell_tester.sh m    # mandatory
./scripts/run_minishell_tester.sh vm   # mandatory + valgrind
```

`MINISHELL_PATH` is set by the script; tester lives in `minishell_tester/` (cloned if missing). See `scripts/run_minishell_tester.sh`.

**Related docs:** [BEHAVIOR.md](BEHAVIOR.md) (bash reference + **minishell deltas**), [TECHNICAL_DECISIONS.md](TECHNICAL_DECISIONS.md), [minishell_architecture.md](minishell_architecture.md).

---

## Latest CI baseline (mandatory + Valgrind job)

**Confirmed twice** with identical numbers (no regression, **no improvement** between snapshots):

| Source | Date (UTC) | Commit / run |
|--------|------------|----------------|
| `logs_61348652699/` | 2026-03-20 | `0a1dea52…` (`lastshell`) |
| Artifact `logs_61469307667` (unzipped) | 2026-03-21 | After **`0b85675`** (docs + `exit` 2 + `logs_*/` ignore); same results as row above |

Both logs: branch **`lastshell`**, job **“Valgrind on Ubuntu (mandatory tests)”**, **`tester.sh m`** (cozyGarage fork).

| Metric | Value |
|--------|--------|
| **TOTAL TEST COUNT** | 944 |
| **TESTS PASSED** | 932 |
| **LEAKING** | 0 |
| **Failed criteria** | STD_OUT **3**, STD_ERR **11**, EXIT_CODE **1** → **15** ❌ (tester counts per check, not per block) |

**Takeaway for next work:** Treat **932 / 944** and the table below as the current target; fixing any row may not change totals until all criteria for that block pass.

**Failing lines (mandatory, same list under Valgrind — leaks still ✅):**

| Script | Line | STD_OUT | STD_ERR | EXIT_CODE | Notes |
|--------|------|---------|---------|-----------|--------|
| `1_builtins.sh` | 552 | ✅ | ❌ | ❌ `minishell(0) bash(127)` | Semicolon / `unset` edge (`;` not in subject) |
| `1_pipelines.sh` | 4, 6, 55, 59, 68, 84, 110 | ✅ | ❌ | ✅ | Pipeline + heredoc stderr vs bash |
| `2_path_check.sh` | 47 | ❌ | ✅ | ✅ | Path / stdout mismatch |
| `9_go_wild.sh` | 7 | ✅ | ❌ | ✅ | Wild / stderr |
| `9_go_wild.sh` | 46, 52 | ❌ | ❌ | ✅ | Wild stdout+stderr |

**Valgrind:** same **15** criterion failures; **LEAKS: ✅** on those lines in the log (no definite leaks reported for mandatory in that run).

Local numbers may differ slightly if tester revision or env differs from CI.

---

## How exit status propagates (“error chain”) — debugging map

Use this when a test fails on **EXIT_CODE** or when `$?` is wrong.

1. **After each line** (`main.c` → `process_input` → …):
   - `shell->last_exit` is set from the **last stage that produced a status** for that line.

2. **Syntax error** (`parser/parser.c` → `parse_input`):
   - `syntax_check()` fails → `shell->last_exit = EXIT_SYNTAX_ERROR` (**2**), tokens freed, **`commands` stay NULL**.
   - Non-interactive: `shell_loop` sees `!commands && last_exit == 2` and **exits the process** with status 2.

3. **Heredoc interrupted** (`process_heredocs` returns non-zero, e.g. SIGINT):
   - `shell->last_exit = 130` (`main.c`), early return (no execute).

4. **Execution** (`execute_commands` → single or pipeline):
   - Return value of `execute_single_command` / `execute_pipeline` becomes **`shell->last_exit`**.
   - **Builtin in parent:** return value of `run_builtin` / `execute_builtin` (e.g. `cd` fail → 1).
   - **External:** parent gets child exit via `waitpid` (127 not found, 126 directory, etc.).
   - **Pipeline:** last command’s exit status (`wait_pipeline`).

5. **Builtin `exit`** (`builtins/exit.c`):
   - **No arg:** `clean_exit(shell, shell->last_exit)` — process exit = previous command’s status.
   - **Numeric arg:** `clean_exit(shell, exit_mod256(args[1]))` — process exit = `n % 256` (with sign rule).
   - **Non-numeric arg:** message to stderr, **`clean_exit(shell, 2)`** — **bash uses 255**; **shell terminates** (not “return 1 and continue”).
   - **Too many args:** stderr + **`return (1)`** — shell **does not** exit; **`last_exit` for that command is 1** (propagates like any builtin return).

6. **Signals at prompt** (`check_signal_received`):
   - Sets `last_exit = 130`, clears `g_signum`.

7. **Expansion `$?`** (`tokenizer/expansion.c`):
   - Uses **`shell->last_exit`** from the **previous** line’s outcome.

8. **Process exit** (`main`):
   - After `shell_loop`, **`return (shell.last_exit)`** — so the **last** `last_exit` from the loop (often last line or syntax break) is the minishell **process** exit code (what the tester compares).

**There is no single global “errno chain”** — errors are: **stderr messages** + **int return** from functions + **`shell->last_exit`** + occasional **`exit()`** from `builtin_exit` / `init_shell` / `msh_calloc` failure.

---

## Session 2026-03-19 — Critical fixes (root causes)

| File | Bug | Impact |
|------|-----|--------|
| `src/tokenizer/tokenizer_utils2.c` | `add_token` missing `tmp->next = new` after the while loop | Multi-token commands, pipes, redirections broken; leaked tokens. |
| `src/core/init.c` | `ft_strdup(NULL)` when `USER` unset (`env -i ./minishell`) | Crash on minimal env. |
| `src/executor/executor.c` | First `dup()` OK, second fails → first FD leaked | FD leak. |
| `src/tokenizer/tokenizer_handlers.c` | Backslash at EOS read past `\0`, advanced `i` by 2 | UB. |
| `src/builtins/export_utils.c` | `find_export_key_index` only matched `KEY=val`, not bare `KEY` | `export`/`unset` wrong for bare keys. |
| `src/builtins/unset.c` | Same pattern in `find_env_index` | Same. |
| `src/executor/executor_external.c` | PATH resolution when `PATH` unset | Intended: align with bash for `unset PATH` / minimal env (see **PATH caveat** below). |

Norm: `norminette src/` clean after these changes.

---

## Session 2026-03-20 — Doc baseline & `exit` code

| Topic | Status |
|--------|--------|
| **Non-numeric `exit`** | Code uses **`clean_exit(shell, 2)`** (bash: **255**). Documented in [BEHAVIOR.md](BEHAVIOR.md), [TECHNICAL_DECISIONS.md](TECHNICAL_DECISIONS.md), architecture. |
| **Structs / docs** | `t_redir`: **`fd` + `append`**, **`REDIR_ERR_OUT`** (`2>`); **`had_path`** on `t_shell`. |
| **Init** | **USER** optional; **`update_shlvl`**; **`had_path`** at startup. |
| **Non-TTY input** | **`read_line_stdin()`** in `main.c`. |

---

### PATH caveat (still verify against bash / tester)

`find_command_path` uses `shell->had_path` (PATH existed at **`init_shell`**). If **`PATH` is removed later** but `had_path` is still true, the code may still search a **built-in default path list** — behavior may **differ from bash** after `unset PATH` in a normal login-style env. If tests fail on “command not found” after `unset PATH`, inspect **`executor_external.c`** + **`init.c`**.

---

## Remaining failure themes (from CI log above)

1. **`1_builtins.sh:552`** — EXIT + STDERR: **minishell(0) vs bash(127)** — semicolon / invalid token after `unset` (subject does not require `;`).
2. **`1_pipelines.sh`** — **STDERR only** on several heredoc+pipe cases — message presence or wording vs bash.
3. **`2_path_check.sh:47`** — **STDOUT** — path resolution / output formatting.
4. **`9_go_wild.sh`** — **STDERR** and **STDOUT** on stress cases.

Older themes (echo backslash, heredoc delimiters, etc.) may still apply on other lines; **refresh** this section after the next `./scripts/run_minishell_tester.sh m`.

---

## Quick reference by script (from last CI)

| Script | In last log | Main cause |
|--------|-------------|------------|
| `1_builtins` | 1 block (552) | `;` / unset / exit 0 vs 127 |
| `1_pipelines` | 7 blocks | STDERR vs bash (heredoc+pipe) |
| `2_path_check` | 1 block (47) | STDOUT |
| `9_go_wild` | 3 blocks (7, 46, 52) | STDERR / STDOUT |

**Replace** with your latest local run if needed.

---

## Is 42_minishell_tester good for bash-like behavior?

**Yes** for mandatory: same input → compare stdout, stderr **presence**, exit code to **bash** on that machine.

- **Subject PDF** is still the authority for scope (`;`, `&&`, etc.).
- **Stderr:** tester often checks presence only, not exact text.
- **CI:** `.github/workflows/valgrind.yml` — Ubuntu, cozyGarage fork of tester.

---

**Local regression suites (optional):** `make -C tests test` runs project scripts (`test_phase1.sh`, `test_hardening.sh`) if present — **not** the same as `42_minishell_tester`.

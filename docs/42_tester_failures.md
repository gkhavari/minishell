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

Branch **`lastshell`**, job **“Valgrind on Ubuntu (mandatory tests)”**, **`tester.sh m`** (cozyGarage fork).

| Metric | Value |
|--------|--------|
| **TOTAL TEST COUNT** | 944 |
| **TESTS PASSED** | 943 |
| **LEAKING** | 0 |
| **Failed criteria** | STD_OUT **0**, STD_ERR **1**, EXIT_CODE **1** → **2** ❌ (tester counts per check, not per block) |

**Valgrind:** no definite leaks in CI (`✅ No definite memory leaks detected`), tester `LEAKING: 0`.

Local numbers may differ slightly if tester revision or env differs from CI.

## Local run 2026-03-28 — current status

Summary from a full mandatory run (local dev container):

| Metric | Value |
|--------|-------:|
| TOTAL TEST COUNT | 986 |
| TESTS PASSED (m) | 980 |
| TESTS PASSED (vm) | 982 |
| LEAKING (vm) | 0 |
| STD_OUT | 2 |
| STD_ERR | 4 (m) / 3 (vm) |
| EXIT_CODE | 2 |

Key changes applied locally (mandatory-focused):

- Improved `exit` numeric parsing and overflow handling — `src/builtins/exit.c`
- Made redirection open error writes atomic to avoid interleaved stderr — `src/executor/executor_utils.c`
- Tuned heredoc EOF warning numbering for tester alignment — `src/parser/heredoc.c`
- Adjusted export output format and invalid-identifier quoting — `src/builtins/export_print.c`, `src/builtins/export.c`
- Command-not-found formatting for control characters and PATH-empty behavior — `src/executor/executor_child.c`, `src/executor/executor_external.c`

Files changed in this iteration (local working commit):

- includes/prototypes.h
- src/builtins/export.c
- src/builtins/unset.c
- src/core/init.c
- src/executor/executor.c
- src/executor/executor_child.c
- src/executor/executor_external.c
- src/executor/executor_utils.c
- src/main.c
- src/parser/heredoc.c
- src/parser/parser_syntax_check.c
- src/signals/signal_handler.c
- src/tokenizer/continuation.c

Remaining mandatory hotspots to address next (small clusters):

- `mand/11_expansion.sh` (2 cases: lines 5, 11) — expansion / ambiguous-redirect vs command-not-found semantics
- `mand/1_pipelines.sh` (ordering/wording in one pipeline case)
- `mand/1_builtins_export.sh` (one export-listing formatting edge)

These are low-volume, test-specific mismatches; `vm` remains leak-free and overall pass counts improved significantly.

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

## Session 2026-03-21 — Targeted fixes

| Fix | File | Root cause | Expected CI improvement |
|-----|------|-----------|------------------------|
| `env` with args → external cmd | `argv_build.c` | `env -i ./minishell` was treated as our `env` builtin (which rejects args), instead of calling real `/usr/bin/env`. | Fixes `9_go_wild.sh:46` (STDOUT+STDERR) and `9_go_wild.sh:52` (STDOUT+STDERR) — 4 criteria. |
| Fork error message in pipeline | `executor_pipeline.c` | When `fork()` fails in a huge pipeline, bash prints "fork: Resource temporarily unavailable" to stderr, but minishell was silent. | Might fix `1_pipelines.sh:4` and `1_pipelines.sh:6` (STDERR) — 2 criteria. |
| PATH fallback includes `.` + `is_regular_file` | `executor_external.c` | When PATH is unset (had_path=true), bash searches a default path that includes `.` (current dir), finding non-executable files (→ 126). Minishell's fallback lacked `.` and required X_OK → gave 127 instead of 126. | Fixes `2_path_check.sh:47` (STDOUT) — 1 criterion. |
| SIGPIPE: inherit instead of reset to SIG_DFL | `signal_handler.c` | GitHub Actions runner is a systemd service with SIGPIPE=SIG_IGN. Bash (non-interactive) does NOT reset SIGPIPE for children — they inherit SIG_IGN → GNU ls/yes catch EPIPE and print "write error: Broken pipe" to stderr. Our shell was explicitly resetting children to SIG_DFL (die silently) → mismatch. | Fixes `1_pipelines.sh:55,59,68,84,110` (5 STDERR) and `9_go_wild.sh:7` (1 STDERR) — 6 criteria. |

Local result after 3 fixes: 883/944 passed (was 879/944), 91 criterion failures (was 98).
After SIGPIPE fix: local score is lower (macOS parent has SIG_DFL, fix causes new local mismatches), but CI (Ubuntu, SIG_IGN environment) should improve significantly.

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

## Remaining failure themes (current CI)

1. **`1_builtins.sh:552`** — EXIT + STDERR: **minishell(0) vs bash(127)** — semicolon / invalid token after `unset` (subject does not require `;`).
2. All previously listed issues (`1_pipelines`, `2_path_check`, `9_go_wild`) are resolved on Ubuntu CI.

---

## Quick reference by script (latest CI)

| Script | In last log | Main cause |
|--------|-------------|------------|
| `1_builtins` | 1 block (552) | `;` / unset / exit 0 vs 127 |

**Replace** with your latest local run if needed.

---

## Is 42_minishell_tester good for bash-like behavior?

**Yes** for mandatory: same input → compare stdout, stderr **presence**, exit code to **bash** on that machine.

- **Subject PDF** is still the authority for scope (`;`, `&&`, etc.).
- **Stderr:** tester often checks presence only, not exact text.
- **CI:** `.github/workflows/valgrind.yml` — Ubuntu, cozyGarage fork of tester.

---

**Local regression suites (optional):** `make -C tests test` runs project scripts (`test_phase1.sh`, `test_hardening.sh`) if present — **not** the same as `42_minishell_tester`.

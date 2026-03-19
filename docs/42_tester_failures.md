# 42_minishell_tester — Status & Known Failures

This document tracks the tester results and known remaining failures.

**CI:** `.github/workflows/valgrind.yml` runs on Ubuntu (`ubuntu-latest`) using the [cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester). Both mandatory (`tester.sh m`) and mandatory+valgrind (`tester.sh vm`) are run.

---

## Session 2026-03-19 — Critical Fixes Applied

### Root-cause bugs fixed (lastshell branch)

| File | Bug | Impact |
|------|-----|--------|
| `src/tokenizer/tokenizer_utils2.c` | `add_token` was missing `tmp->next = new;` after the while loop | **ALL** multi-token commands, pipes, redirections silently broken. Every token after the first was leaked and invisible. |
| `src/core/init.c` | `ft_strdup(NULL)` crash when `USER` env var not set (e.g. `env -i ./minishell`) | Crash on clean environments. |
| `src/executor/executor.c` | When first `dup()` succeeded but second failed, the first FD was leaked | FD leak on dup failure path. |
| `src/tokenizer/tokenizer_handlers.c` | Backslash at end of string read `input[i+1]` past `\0` then advanced `i` by 2 | UB / potential read past end. |
| `src/builtins/export_utils.c` | `find_export_key_index` only matched `KEY=val` entries, not bare `KEY` | `export VAR` without value couldn't be found/updated. |
| `src/builtins/unset.c` | Same as above in `find_env_index` | `unset` couldn't find bare `KEY` entries. |
| `src/executor/executor_external.c` | Hardcoded fallback PATH when `PATH` unset → commands still found | `unset PATH; ls` should give 127, not 0. |

All fixes are Norm-compliant (`norminette src/` shows zero errors).

### Expected result after these fixes

The previous baseline (before fixes) was **888/941 passed**. The `add_token` bug alone broke every command with more than one token — the fact that 888 passed at all was because many single-token tests still worked. After the fix, essentially all basic command, pipe, redirection, and builtin tests should pass, pushing the score significantly higher.

---

## Is the 42_minishell_tester good for checking bash-like behavior? (Mandatory part)

**Short answer:** Yes, for the mandatory part it's a solid way to check that your shell behaves like bash on the same inputs. It's not the subject PDF, and it has some limits, but it's a useful reference.

### How the mandatory tests work

- The tester reads each test block from the scripts under `cmds/mand/`.
- For each block it:
  1. Pipes the **same input** into your minishell and captures stdout, stderr, and exit code.
  2. Pipes the **same input** into **bash** and captures stdout, stderr, and exit code.
  3. Compares:
     - **Stdout:** `diff -q` (byte-for-byte).
     - **Stderr:** Only whether both have stderr or both don't (presence), not exact text.
     - **Exit code:** Must match exactly.

So the reference is **"bash on this machine"** running the same script.

### Strengths

- **Bash as reference:** Same input → minishell vs bash; good for "behaves like bash" on that input.
- **Broad coverage:** Mandatory tests cover builtins, parsing, redirections, heredocs, pipelines, variables, path resolution, syntax errors, and edge cases (941 cases in mandatory).
- **Structured:** You can run subsets (e.g. `m b` = builtins only) to narrow down failures.
- **Valgrind mode:** `vm` runs the same tests under Valgrind for leak checks.

### Limitations and caveats

- **Subject PDF is the authority:** The tester is third-party. Some tests may go beyond the PDF (e.g. `>|`, `;`); failing them doesn't automatically mean you're wrong per subject.
- **Bash version:** It uses whatever `bash` is in `PATH` (on macOS often 3.x, on Linux 4.x/5.x). Running on Ubuntu (CI) aligns with the usual 42 evaluation environment.
- **Stderr:** Only presence is checked, not the exact message.

---

## Remaining known failures (post-fix)

These are expected to still fail on the current codebase. They require behavior changes beyond the scope of the critical fixes.

### 1. Echo / backslash and quote handling (`0_compare_parsing`, `1_builtins`)

**Examples:** `echo \$USER`, `echo \\$USER`, `echo \\\$USER`, etc.

**Why:** Backslash escaping in unquoted context doesn't fully match bash. Outside quotes, bash treats `\X` as literal `X` (stripping the backslash). Our tokenizer may behave differently.

---

### 2. Heredoc edge cases (`1_redirs`, `1_pipelines`, `10_parsing_hell`)

**Examples:**
- `cat << lim''`, `cat << "lim"`, `cat << 'lim'` — quoted delimiters
- `cat << $USER` — variable in delimiter
- Heredocs mixed with pipes: `ls | cat << stop | grep "asd"`

**Why:** Delimiter quoting (quoted delimiter = no expansion in body) and heredoc+pipe ordering may not fully match bash.

---

### 3. Builtins: cd, export, exit edge cases (`1_builtins`)

**Examples:**
- `cd '/////'` — bash normalizes to `/` and exits 0
- `export TEST+=100` (invalid name format) — bash exits 0, we exit 1
- `exit 123"123"` — bash exits 127, we exit 2

**Why:** Edge case behavior differences in path normalization and argument validation.

---

### 4. Semicolons (`1_builtins:552`)

**Command:** `unset TES;T` — bash parses as `unset TES` then `T` (exits 127); minishell doesn't implement `;` separator.

**Note:** `;` is not required by the minishell subject (v10). This is a known and acceptable difference.

---

### 5. Exit code 126 in pipelines (`1_pipelines:156,161`)

**Command:** `ls|cat Makefile|cat<<"asd">out` — minishell exits 126, bash exits 0.

**Why:** Heredoc with double-quoted delimiter in a pipeline likely triggers wrong exec path or heredoc FD not set up correctly for child.

---

## Quick reference by script

| Script               | Est. failures | Main cause |
|----------------------|---------------|------------|
| 0_compare_parsing    | ~7            | Echo backslash/quote/expansion vs bash |
| 1_builtins           | ~5            | cd/export/exit edge cases |
| 1_pipelines          | ~3            | Heredoc+pipe, quoted heredoc delimiter |
| 1_redirs             | ~9            | Heredoc delimiter/expansion |
| 8_syntax_errors      | 0             | `>|` is now implemented |
| 10_parsing_hell      | ~3            | Heredoc, expansion |

Estimated total remaining: **~27 failures** (vs 888/941 before the critical fixes). Actual number will be confirmed by CI run.

---

**Test suite:** This project uses only **42_minishell_tester** ([cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester)). Run with `make -C tests test`. Expected behavior and test-design guidance are in [BEHAVIOR.md](BEHAVIOR.md).

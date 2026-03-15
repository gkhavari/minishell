# 42_minishell_tester — Why Some Tests Fail

This document summarizes **representative** failed test cases from `./scripts/run_minishell_tester.sh m` and the likely reasons they differ from bash.

**Overall:** 888 passed, 90 failed (941 total). No leaks. Failures fall into a few categories.

---

## Is the 42_minishell_tester good for checking bash-like behavior? (Mandatory part)

**Short answer:** Yes, for the mandatory part it’s a solid way to check that your shell behaves like bash on the same inputs. It’s not the subject PDF, and it has some limits, but it’s a useful reference.

### How the mandatory tests work

**CI and fork:** The repo uses the [cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester) of 42_minishell_tester in CI (valgrind workflow runs mandatory tests with and without Valgrind). The fork can be updated with extra or subject-aligned tests when needed.

- The tester reads each test block from the scripts under `cmds/mand/`.
- For each block it:
  1. Pipes the **same input** into your minishell and captures stdout, stderr, and exit code.
  2. Pipes the **same input** into **bash** (with a leading `enable -n .` line so bash runs the test as the last command) and captures stdout, stderr, and exit code.
  3. Compares:
     - **Stdout:** `diff -q` (byte-for-byte).
     - **Stderr:** Only whether both have stderr or both don’t (presence), not exact text.
     - **Exit code:** Must match exactly.

So the reference is **“bash on this machine”** running the same script. If all three match, the test passes.

### Strengths

- **Bash as reference:** Same input → minishell vs bash; good for “behaves like bash” on that input.
- **Broad coverage:** Mandatory tests cover builtins, parsing, redirections, heredocs, pipelines, variables, path resolution, syntax errors, and edge cases (941 cases in mandatory).
- **Structured:** You can run subsets (e.g. `m b` = builtins only) to narrow down failures.
- **Valgrind mode:** `vm` runs the same tests under Valgrind for leak checks.
- **Widely used:** 42_minishell_tester (zstenger93) is a common choice for minishell; many students rely on it.

### Limitations and caveats

- **Subject PDF is the authority:** The tester is third-party. It does not replace the mandatory requirements in the subject. Some tests may go beyond the PDF (e.g. `>|`, `;`); failing them doesn’t automatically mean you’re wrong per subject. Use the tester as a strong hint, not the final definition.
- **Bash version:** It uses whatever `bash` is in `PATH` (on macOS often 3.x, on Linux 4.x/5.x). So “like bash” means “like that bash.” Running on Ubuntu (e.g. CI) aligns with the usual 42 evaluation environment.
- **Stderr:** Only presence is checked, not the exact message. So your error text can differ from bash and still pass, as long as both have stderr or both don’t.
- **No mapping to subject:** The scripts don’t tag which subject bullet each test covers. You may need to cross-check with the PDF to ensure every mandatory point is exercised.
- **Possible quirks:** A few test lines might be ambiguous or depend on bash-specific behavior. Your `docs/42_tester_failures.md` notes those (e.g. `>|`, `;`).

### Verdict

- **For the mandatory part:** The suite is **good to verify “does my shell behave like bash?”** on the same inputs: same stdin, compare stdout (exact), stderr (presence), and exit code. It’s a strong sanity check and catches many real bugs.
- **Best used together with:** (1) the official subject PDF for what’s truly required, and (2) your own tests or the project’s local tests (e.g. `make -C tests test`) for behavior you care about most.
- **To trust it more:** Run it on Linux/Ubuntu (e.g. in CI) so the reference bash matches the evaluation environment; that also avoids macOS vs Linux bash differences.

---

## Code vs platform (macOS vs Linux/Ubuntu)

- **Most failures are from your code** (parsing, builtin behavior, exit codes, PATH/exec in children). The tester runs the same script on the same machine for both bash and minishell; if minishell’s exit code or output differs, it’s because minishell behaves differently, not because the OS is different.
- **Possible platform effects** (usually small):
  - **PATH:** The tester may set or inherit PATH. On macOS, `/bin` and `/usr/bin` exist; on Ubuntu too. If the tester runs with an empty or minimal PATH, both shells get it; then 127 (command not found) is expected for `ls`/`cat` unless the test restores PATH. So 127 in bulk is still “minishell doesn’t find the command” (code), unless the tester explicitly differs between macOS and Linux.
  - **Exit 126** (not executable): Could in theory differ if a test uses a script and exec/shebang behavior differs (e.g. `#!/bin/bash` vs `#!/usr/bin/env bash`), but the mandatory tests don’t rely on that. So 126 is still treated as code (minishell returning 126 where bash returns 0).
- **Conclusion:** Treat all listed exit-code and output differences as **code vs bash (Linux) behavior**. Re-run the tester on Ubuntu (e.g. CI or a Linux VM) to confirm; the numbers may change slightly (e.g. PATH), but the “minishell vs bash” mismatch types stay the same.

---

## All failures where EXIT_CODE differs from bash

Below, **minishell(N) vs bash(M)** means: for that test line, minishell exits with N and bash with M. “Likely cause” is code unless noted.

| Script:Line | Command (summary) | minishell | bash | Likely cause |
|-------------|-------------------|-----------|------|--------------|
| **8_syntax_errors.sh:100** | `>| echo wtf` | 2 | 0 | **Code.** `>|` not supported; minishell syntax error. |
| **10_parsing_hell.sh:32** | `cat << $USER` (heredoc, empty or set) | 127 | 0 | **Code.** Command not found in heredoc context (PATH/exec or heredoc handling). |
| **10_parsing_hell.sh:40** | `$T lol` (T=">>") → redirect + "lol" | 127 | 0 | **Code.** Parsing/exec of redirect-only or “lol” as command. |
| **10_parsing_hell.sh:319** | `exit 123"123"` | 2 | 127 | **Code.** Exit builtin: minishell reports syntax/usage (2), bash runs as command (127). |
| **1_builtins.sh:233** | `cd '/////' 2>/dev/null` | 1 | 0 | **Code.** cd to `/////`; bash normalizes to `/` and succeeds; minishell fails. |
| **1_builtins.sh:490** | `export TEST+=100` (invalid name) | 1 | 0 | **Code.** Export validation; bash may accept or ignore, minishell returns error. |
| **1_builtins.sh:552** | `unset TES;T` (semicolon in name / two commands) | 1 | 127 | **Code.** Parsing: bash runs `unset TES` then `T` (127); minishell parses differently and exits 1. |
| **1_pipelines.sh:55** | `ls | cat asd` | 127 | 1 | **Code.** Pipeline: minishell 127 = can’t find `cat`; bash runs cat, gets 1 (file not found). |
| **1_pipelines.sh:59** | `ls | cat << stop \| grep "asd"` | 127 | 0 | **Code.** Heredoc in pipeline; minishell doesn’t find command. |
| **1_pipelines.sh:68** | pipeline with heredocs | 127 | 0 | **Code.** Same. |
| **1_pipelines.sh:84** | pipeline with heredocs | 127 | 0 | **Code.** Same. |
| **1_pipelines.sh:110** | pipeline with heredocs | 127 | 0 | **Code.** Same. |
| **1_pipelines.sh:151** | `ls|cat Makefile|cat<<asd>out` | 127 | 0 | **Code.** Heredoc + redir in pipeline. |
| **1_pipelines.sh:156** | `ls|cat Makefile|cat<<'asd'>out` | 126 | 0 | **Code.** Minishell 126 (not executable) vs bash 0. |
| **1_pipelines.sh:161** | `ls|cat Makefile|cat<<"asd">out` | 126 | 0 | **Code.** Same. |
| **1_redirs.sh:186** | `cat << lim` (heredoc) | 127 | 0 | **Code.** Minishell doesn’t find `cat` in heredoc. |
| **1_redirs.sh:195** | `<<lim cat` (heredoc before cmd) | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:204** | `cat << lim` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:208** | `cat << EOF` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:217** | `cat << hello` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:223** | `cat << 'lim'` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:229** | `cat << lim''` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:235** | `cat << "lim"` | 127 | 0 | **Code.** Same. |
| **1_redirs.sh:241** | `cat << 'lim'` (again) | 127 | 0 | **Code.** Same. |
| **1_scmds.sh:19** | `cat cmds.sh` | 127 | 0 | **Code.** Simple cat; minishell doesn’t find `cat` (PATH/exec in child). |

**Total: 25 test lines** where the exit code differs from bash. All are classified as **code** (minishell behavior vs bash on the same OS). None are clearly “macOS vs Linux” only; re-running on Ubuntu (e.g. CI) will show the same mismatch types; only if the tester or env differs per-OS could a few numbers change.

---

## Are the exit-code fixes easy or complicated?

The table makes it look like "just change the exit code," but the real work is fixing the **behavior** that produces that code. Here's the breakdown.

### Easier / scoped

- **`>|` (8_syntax_errors:100)** — One operator: parse `>|` and treat it like `>` (force overwrite). Bounded change in lexer/parser and redirection handling.
- **`cd '/////'` (1_builtins:233)** — Bash normalizes to `/` and exits 0. In your `cd` builtin, normalize the path (e.g. collapse `//`, resolve `.`/`..`) before `chdir()`, or use `realpath()`. Clear task, moderate work.
- **Export / exit edge cases (1_builtins:490, 10_parsing_hell:319)** — Depends on current rules. Match bash for `export TEST+=100` and `exit 123"123"` (validation/parsing). Medium effort, well-defined.

### One fix can fix many (127 "command not found")

About **18 of the 25** exit-code failures are **minishell 127 vs bash 0** (e.g. `cat cmds.sh`, `cat << lim`, `ls | cat asd`). The usual cause is that **children don't see the same environment (especially PATH)** as bash's children:

- The executor doesn't pass the shell's environment into `execve`/`execvp`, or
- The environment is built without `PATH`, or cleared somewhere before exec.

Fixing "pass the real env (including PATH) into the exec call" is **one conceptual fix** that can fix most of those 127s. The catch is finding **where** the env is lost (main loop → executor → `exec`), which can take some tracing.

### More involved

- **`unset TES;T` (1_builtins:552)** — Bash runs two commands; exit code is from `T` (127). If your subject doesn't implement `;` as a command separator, "fixing" this means adding that syntax. That's a design/scope decision, not a small tweak.
- **126 in pipelines (1_pipelines:156, 161)** — Minishell 126, bash 0. Usually "not executable" or wrong exit-code propagation. Need to see what minishell is trying to exec and why it gets 126 (wrong path, or returning 126 instead of the child's status). A bit fiddly to debug.

**Summary:** Some fixes are easy or scoped (`>|`, cd normalization, export/exit rules). The 127s often share a single root cause (env not passed to exec); fixing that can clear many at once. Others (semicolon, 126 in pipelines) are not trivial. The exit code is the symptom; fixing it is as easy or as hard as the underlying cause.

---

## 1. **Echo / backslash and quote handling** (0_compare_parsing, 1_builtins)

**Scripts:** `0_compare_parsing.sh` (lines 52–64), `1_builtins.sh` (lines 59–71, 314, 324, 347, 390)

**Examples:**
- `echo \$USER`   → bash: literal `$USER`; minishell may output `\` + expansion or different escaping
- `echo \\$USER`, `echo \\\$USER`, `echo \\\\$USER`, etc.
- `echo "cat lol.c | cat > lol.c"`
- `echo $USER =intergalaktikus miaf*szomez`  → word splitting / globbing

**Why it fails:** Stdout is compared to bash. Differences come from:

- **Backslash escaping** in the echo builtin (e.g. `\` before `$` or other characters).
- **Quote handling** (single vs double, what is literal vs expanded).
- **Field splitting and globbing** (e.g. `*`), or lack of them.

Fixing this means aligning your echo (and possibly expansion/splitting) rules with bash for these cases.

---

## 2. **Syntax: `>|` (clobber)** (8_syntax_errors)

**Script:** `8_syntax_errors.sh` line 100

**Command:** `>| echo wtf`  
(Before it the test does `/bin/rm -rf echo` so there is no `echo` binary; the builtin is meant.)

**Why it fails:**

- **Bash:** `>|` is valid (overrides `noclobber`); redirect then run `echo wtf` → exit **0**.
- **Minishell:** treats `>|` as a syntax error → exit **2**.

So the tester sees **EXIT_CODE: minishell(2) vs bash(0)**.

**Fix:** Either implement `>|` as a redirection operator, or accept this as a known difference (many subjects don’t implement `>|`).

---

## 3. **Empty environment / PATH** (9_go_wild, 2_path_check, 1_pipelines, 1_redirs, 1_scmds, 10_parsing_hell)

**Scripts:**  
`9_go_wild.sh` (46, 49, 52), `2_path_check.sh` (43, 47), `1_pipelines.sh`, `1_redirs.sh`, `1_scmds.sh`, `10_parsing_hell.sh`

**Typical commands:**
- `echo "ls" | env -i ./minishell`  → minishell gets no PATH, so `ls` → command not found (127).
- `tmp_x_file1` or `./tmp_x_file1` with PATH unset or minimal.
- Heredocs and pipelines run under `env -i` or with PATH cleared.

**Why it fails:**

- With **env -i**, there is no PATH. Commands like `ls` or `cat` are not found → exit **127** and an error message.
- Tester compares **stdout/stderr and exit code** to bash. Bash’s error message format (e.g. `bash: ls: command not found`) can differ from minishell’s (e.g. `minishell: ls: command not found` or different wording).
- Some tests may expect bash to find binaries (e.g. via a minimal PATH or different env), so exit codes (127 vs 0) and output differ.

**Fix:** Align behavior with empty env: same exit codes (127 for “command not found”) and, if required by the tester, similar error message format. Optionally ensure a minimal PATH in certain tests if the tester assumes it.

---

## 4. **Heredocs** (1_redirs, 10_parsing_hell, 1_pipelines)

**Scripts:** `1_redirs.sh` (e.g. 186–241), `10_parsing_hell.sh`, `1_pipelines.sh`

**Examples:**
- `cat << lim` with delimiter in quotes or with trailing text: `lim''`, `"lim"`, `'lim'`.
- `cat << $USER`, `cat << "$USER"`, `cat << "$US"E"R"`.
- Heredocs mixed with pipes: `ls | cat << stop | grep "asd"`.
- Expansion (or no expansion) inside heredoc body: `cat << 'lim'` vs `cat << lim`.

**Why it fails:**

- Delimiter quoting and parsing (quoted vs unquoted, `lim` vs `lim''` vs `"lim"`) may not match bash.
- Variable expansion in delimiter (`$USER`, `"$USER"`) or in body when delimiter is unquoted.
- Exit 127 when `cat` is not in PATH (see §3).
- Order/behavior of heredoc + pipe + redirection can differ.

**Fix:** Implement heredoc parsing and expansion rules to match bash (delimiter recognition, quoting, expansion in body vs delimiter).

---

## 5. **Builtins: export, cd, exit** (1_builtins)

**Script:** `1_builtins.sh` (e.g. 233, 239, 243, 490, 552)

**Examples:**
- `cd '/////'`  → stderr or exit: minishell(1) vs bash(0) (e.g. one treats invalid path as error, the other doesn’t).
- `cd bark bark`  → cd with extra args: minishell may error, bash ignores extra args.
- `export` with invalid name or `exit` with too many args → different exit code or stderr.

**Why it fails:**

- **cd** with multiple arguments or invalid path: bash often exits 0 and may normalize path; minishell may return 1 and print to stderr.
- **export** / **exit** edge cases: exit code or stderr format differ from bash.

**Fix:** Match bash for cd (extra args, `/////`), export (invalid names), and exit (too many args, non-numeric) and, if the tester checks it, stderr wording.

---

## 6. **Exit code differences** (1_builtins, 10_parsing_hell)

**Examples:**
- `10_parsing_hell.sh:319` — EXIT_CODE minishell(2) vs bash(127): one reports syntax error, the other “command not found”.
- `1_builtins.sh:552` — minishell(1) vs bash(127): e.g. exit with bad args vs command-not-found.

**Why it fails:** Same command is classified differently (syntax error vs command not found vs invalid usage), so exit codes don’t match.

**Fix:** Decide which behavior is desired and align exit codes (and optionally stderr) with bash for those cases.

---

## Quick reference by script

| Script               | Failures | Main cause |
|----------------------|----------|------------|
| 0_compare_parsing    | 7        | Echo backslash/quote/expansion vs bash |
| 1_builtins           | 15+      | Echo, cd/export/exit edge cases, exit codes |
| 1_pipelines          | 8        | Heredoc+pipe, empty PATH (127) |
| 1_redirs             | 9        | Heredoc delimiter/expansion, PATH |
| 1_scmds              | 1        | Exit code with empty/minimal env |
| 2_path_check         | 2        | PATH / `env -i` behavior |
| 2_correction         | 1        | Stdout vs bash |
| 8_syntax_errors      | 1        | `>|` not supported (exit 2 vs 0) |
| 9_go_wild            | 3        | env -i, PATH, stdout for `ls` / `unset PATH` |
| 10_parsing_hell      | 5        | Heredoc, expansion, exit 127 vs 2 |

To debug a specific case, run the failing command from the script line in both bash and minishell and compare stdout, stderr, and exit code.

---


**Test suite:** This project uses only **42_minishell_tester** ([cozyGarage fork](https://github.com/cozyGarage/42_minishell_tester)). Run with `make -C tests test`. Expected behavior and test-design guidance are in [BEHAVIOR.md](BEHAVIOR.md).

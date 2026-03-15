# Minishell Behavior & Edge Cases

This document describes the **intended and tested** behavior of minishell. All behaviors listed here are covered by the test suite: **LucasKuhn/minishell_tester** plus the project’s **tests/local_tests** (consolidated from former phase1, hardening, and behavior). Run with `make -C tests test` from the repository root.

For architecture and data structures, see [minishell_architecture.md](minishell_architecture.md).

---

## Summary: What Is Implemented vs Not

| Feature | Status | Notes |
|--------|--------|--------|
| Prompt, readline, history | ✅ | TTY vs non-TTY (silent prompt). |
| Pipes `|` | ✅ | Pipeline exit = last command's exit. |
| Redirections `<` `>` `>>` `<<` | ✅ | Left-to-right; heredoc before execution. |
| Variable expansion `$VAR`, `$?` | ✅ | In double quotes; not in single quotes. |
| Builtins: echo, cd, pwd, env, export, unset, exit | ✅ | See §4. |
| Syntax errors (pipe/redir) | ✅ | Exit 2, message to stderr. |
| Path resolution, 127/126 | ✅ | Absolute path, PATH search. |
| **`&&` / `||` / `;`** | ❌ | **Not implemented** (out of 42 mandatory scope). Use multiple lines for chaining. |

---

## 1. Input Resilience (No Crash)

The shell must not segfault or abort on invalid or empty input. **Verified by:** Hardening §1, §2, §13.

| Input | Expected behavior |
|-------|-------------------|
| Empty string `""` | No crash; exit 0. |
| Spaces/tabs only | No crash; exit 0. |
| Many blank lines | No crash; exit 0. |
| Lone pipe `|` | Syntax error; exit 2; no crash. |
| Double pipe `||` | Syntax error; exit 2; no crash. |
| Pipe at end `echo hi |` | Syntax error; exit 2; no crash. |
| Unclosed double quote `echo "hello` | Handled (continuation or no crash). |
| Unclosed single quote `echo 'hello` | Handled (continuation or no crash). |
| Redirection with no file `echo hi >` | Syntax error; exit 2; no crash. |
| Multiple redirects no file `>> >> >>` | Syntax error; exit 2; no crash. |
| Heredoc with no delimiter `cat <<` | Syntax error; exit 2; no crash. |
| Only redirection token `>` | Syntax error; exit 2; no crash. |
| Empty command (only spaces) | No crash; exit 0. |
| Pipe first `| echo hi` | stderr contains "syntax"; exit 2. |
| Pipe last `echo hi |` | stderr contains "syntax"; exit 2. |
| Expansion at end `echo $` | No crash; exit 0. |

---

## 2. Redirections (`<`, `>`, `>>`, `<<`)

**Verified by:** Phase 1 (indirectly via builtins); Hardening §7, §13, §16.

### 2.1 Evaluation order

- Redirections are applied **left-to-right** before the command runs.
- **Multiple output redirections** (e.g. `echo hi > a > b`): both files are opened; stdout ends up on the **last** one (only `b` contains "hi").
- **Input redirection**: If the file is missing, the shell prints an error (e.g. `minishell: ... No such file or directory`), does **not** run the command, and exits with status **1**.

### 2.2 Tested cases

| Command / scenario | Expected |
|--------------------|----------|
| `echo hello > /tmp/f ; cat /tmp/f` | File contains `hello`; then `cat` prints it. |
| `echo line1 > /tmp/f ; echo line2 >> /tmp/f ; cat /tmp/f` | File contains `line1` and `line2`. |
| `echo hello > /tmp/f ; cat < /tmp/f` | Output: `hello`. |
| `echo hi > /no_such/dir/file.txt` | stderr: "No such file" (or similar). |
| `cat < /no_such_file_xyz` | stderr: "No such file"; exit 1. |
| `echo hello \| cat > /tmp/out` then `cat /tmp/out` | File contains `hello`. |
| `echo one > /tmp/f ; echo two >> /tmp/f ; cat < /tmp/f` | Output: `one` then `two`. |
| Redirect to directory `echo x > /tmp` | Fails (exit 1); no crash. |

### 2.3 Heredocs (`<<`)

- Heredocs are read and processed **before** execution (during parsing/heredoc phase).
- **Variables** are expanded inside the heredoc body **unless** the delimiter is quoted (e.g. `<< 'EOF'` or `<< "EOF"`).
- **Verified by:** Hardening §8 (heredoc basic, heredoc expands vars, heredoc no expand quoted, empty body, EOF).

| Example | Expected |
|---------|----------|
| `cat << EOF` then `hello` then `EOF` | Output: `hello`. |
| `export HD_VAR=world` then `cat << EOF` then `hello $HD_VAR` then `EOF` | Output: `hello world`. |
| `cat << 'EOF'` then `hello $HOME` then `EOF` | Output: `hello $HOME` (literal). |
| Empty body `cat << DELIM` then `DELIM` | No crash. |
| Heredoc then another command (e.g. `cat << END` … `END` then `echo done`) | Heredoc then `echo done` both run. |

---

## 3. Pipes (`|`)

**Verified by:** Hardening §8, §17.

- Every command in a pipeline runs in a **child process**. Builtins in the middle of a pipeline (e.g. `cd /tmp | cat`) do **not** change the parent shell’s environment.
- **Exit status** of the pipeline is the exit status of the **last** command.

| Command | Expected stdout | Expected exit |
|---------|-----------------|---------------|
| `echo hello | cat` | `hello` | 0 |
| `echo hello | cat | cat` | `hello` | 0 |
| `echo hello | cat | cat | cat | cat | cat` | `hello` | 0 |
| `printf 'foo\nbar\nbaz\n' | grep bar` | `bar` | 0 |
| `printf 'a\nb\nc\n' | wc -l` | `3` (or equivalent) | 0 |
| `echo foo | cat` | `foo` | 0 |
| `echo  a  b  c  | cat` | `a  b  c` (spaces preserved as in bash) | 0 |
| `true | false` | (none) | **1** |
| `false | true` | (none) | **0** |
| `echo x | cat > /tmp/f` then `cat /tmp/f` | File contains `x`. |

- **Syntax:** Pipe first (e.g. `| cat`) or pipe last (e.g. `echo hi |`) → syntax error, exit **2**, message containing "syntax".
- Long pipelines (e.g. 20× `cat`) must not crash.

---

## 4. Variable Expansion (`$`)

**Verified by:** Phase 1 (export/set/echo); Hardening §4, §15.

- **Unset variable** → expanded to **empty string** (no crash). Example: `echo $TOTALLY_UNDEFINED_XYZ_VAR` → empty line.
- **`$` alone** (end of word) → literal `$`. Example: `echo $` → `$`.
- **`$?`** → exit status of the last command. Example: after `true`, `echo $?` → `0`; after `false`, `echo $?` → `1`.
- **`$1`** (digit immediately after `$`) → not expanded; literal `$1` (or equivalent).
- **Single quotes** → no expansion. Example: `echo '$HOME'` → `$HOME`.
- **Double quotes** → variables expanded. Example: `echo "hello $VAR"` after `export VAR=world` → `hello world`.
- **Set and echo:** `export MY_TEST_VAR=hello` then `echo $MY_TEST_VAR` → `hello`.
- **Unset:** `export UNSET_TEST=xyz` then `unset UNSET_TEST` then `echo $UNSET_TEST` → empty.
- **Underscore in name:** `export A_B=1` then `echo $A_B` → `1`.
- **Invalid export name** (e.g. `export A-B=x`) → stderr "not a valid identifier"; exit 1.

---

## 5. Builtins

**Verified by:** Phase 1 §2–§8; Hardening §3, §6, §10, §14.

### 5.1 echo

- **echo** prints arguments separated by spaces and a trailing newline, unless `-n` is used.
- **-n** suppresses the trailing newline. Multiple `-n` (e.g. `-n`, `-nnnn`) are accepted; parsing of `-n` stops at the first non-flag argument (e.g. `echo -n -a hello` → prints `-a hello` with newline).

| Input | Output |
|-------|--------|
| `echo hello` | `hello` + newline |
| `echo hello world` | `hello world` + newline |
| `echo -n hello` | `hello` (no newline) |
| `echo -n -n hello` | `hello` (no newline) |
| `echo -nnnnn hello` | `hello` (no newline) |
| `echo` | newline only |
| `echo ""` | newline only |
| `echo -n` | nothing |
| `echo -n -a hello` | `-a hello` + newline |
| `echo 'hello world'` | `hello world` + newline |
| `echo "hello world"` | `hello world` + newline |

### 5.2 pwd

- Prints the current working directory (e.g. from `getcwd()` or shell’s `cwd`). **Verified by:** Phase 1 pwd basic; Hardening pwd, cd /tmp and pwd, cd HOME.

### 5.3 cd

- **cd** with a path changes the current directory and updates `PWD` and `OLDPWD` in the environment.
- **cd** with no arguments changes to `$HOME`.
- **cd -** changes to `$OLDPWD` and prints the new path.
- **cd** to a non-existent path → stderr "No such file or directory"; exit **1**.
- **cd with extra args** (e.g. `cd /tmp /var`): only the first argument is used as the target; subsequent args are ignored (behavior matches bash for "first arg only"). **Verified by:** Hardening "cd with extra args" (path comparison after `cd /tmp /var` then `pwd`).

### 5.4 env

- **env** with no arguments prints the environment (one `VAR=value` per line). **Verified by:** Phase 1 env contains PATH/HOME/USER; Hardening env has PATH/HOME, export sets var in env, unset removes from env.

### 5.5 export

- **export** with no arguments prints exported variables in **declare -x** form (bash-compatible). **Verified by:** Phase 1 "export no args shows vars"; Hardening "export no args has declare".
- **export NAME=value** sets and exports the variable. **Verified by:** Phase 1 "export set var"; Hardening "export sets var in env".
- **Invalid name** (e.g. starts with digit or contains `-`): stderr "not a valid identifier"; exit **1**. **Verified by:** Phase 1 "export invalid name"; Hardening "export invalid name", "export bad name exit 1", "invalid export".

### 5.6 unset

- **unset VAR** removes the variable from the environment. A following **env** must not show it. **Verified by:** Phase 1 "unset removes var"; Hardening "unset removes from env".

### 5.7 exit

- **exit** with no arguments exits the shell with the **last command’s exit status** (e.g. 0 if last succeeded).
- **exit n** (numeric): shell exits with `n % 256`. Examples: `exit 0` → 0; `exit 42` → 42; `exit 255` → 255; `exit 256` → 0; `exit 257` → 1; `exit -1` → 255.
- **exit** with **non-numeric** argument: stderr "numeric argument required"; shell exits with **255**.
- **exit** with **too many arguments** (e.g. `exit 1 2` or `exit 1 2 3`): stderr "too many arguments"; shell does **not** exit; exit status of that command is **1**.

**Verified by:** Phase 1 exit no args/0/42/255; Hardening exit 0/42/255, exit 256 wraps, exit no args, exit non-numeric, exit too many args no exit, exit negative, exit 257 wraps to 1, exit too many args exit 1.

---

## 6. Exit Codes (Bash-Aligned)

**Verified by:** Phase 1 exit tests; Hardening §10, §11, §14, §17.

| Code | Meaning | Example |
|------|---------|---------|
| 0 | Success | Successful command or `exit 0`, `exit 256`. |
| 1 | General error / builtin failure | `cd` to bad dir, `export` invalid name, `exit 1 2`, pipeline last command failed. |
| 2 | Syntax error | Pipe first/last, redirection with no file. |
| 126 | Command not executable | Running a directory as command (e.g. `/tmp`). |
| 127 | Command not found | Unknown command name. |
| 128 + N | Killed by signal N | e.g. 130 = SIGINT, 131 = SIGQUIT. |
| 255 | exit with non-numeric arg | Shell exits 255 after printing error. |

---

## 7. Path Resolution

**Verified by:** Hardening §11.

- **Absolute path** (e.g. `/bin/echo hello`): executed directly if executable.
- **Relative path**: resolved relative to current directory.
- **Name without slash**: searched in `PATH`; if not found → stderr "command not found", exit **127**.
- If the resolved path is a **directory** (e.g. `/tmp` run as command): exit **126**, error to stderr.

---

## 8. Signals

- **Ctrl+C (SIGINT):** At the prompt, prints a newline and shows a new prompt; `$?` becomes 130. During command execution, child is interrupted and exit status 130.
- **Ctrl+D (EOF):** Exits the shell (after printing "exit" in interactive mode).
- **Ctrl+\ (SIGQUIT):** Ignored at the prompt; during execution, terminates the child (e.g. "Quit"), exit status 131.

Manual checks only in current suites; behavior is aligned with the architecture doc.

---

## 9. Multi-Line Input

- When input is fed line-by-line (e.g. via pipe or redirect), each **line** is processed as a separate command. So you can simulate "chaining" without `&&` by sending multiple lines: e.g. `cd /tmp` then `pwd` → second line sees the new directory. **Verified by:** Phase 1 cd/pwd and export/env use multi-line input; Hardening cd /tmp and pwd, export then env, etc.

---

## 10. Cross-Reference to Tests

| Test suite | Section in script | Covers |
|------------|-------------------|--------|
| Phase 1 | Echo | echo basic, -n, empty, -n -a. |
| Phase 1 | PWD, CD | pwd, cd then pwd, cd nonexistent. |
| Phase 1 | ENV, Export, Unset | env, export, export set var, invalid name, unset. |
| Phase 1 | Exit | exit 0/42/255, exit no args. |
| Hardening | §1 Empty/whitespace | No crash on empty, spaces, tabs, blank lines. |
| Hardening | §2 Syntax resilience | Pipe, unclosed quotes, redir no file, heredoc no delim. |
| Hardening | §3–§6 | Echo, expansion, cd, export/env/unset. |
| Hardening | §7–§8 | Redirections, pipes, heredocs. |
| Hardening | §10–§11 | Exit codes, path (127, 126). |
| Hardening | §13–§17 | Parsing/syntax, builtin exit codes, variables, redir edge cases, pipelines. |

For full test names and commands, see the scripts under `tests/`.

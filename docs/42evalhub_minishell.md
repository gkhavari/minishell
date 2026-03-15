# 42 EvalHub — minishell (Adapted Evaluation Checklist)

Source: https://www.42evalhub.com/common/minishell

> This file is an adapted, repository-specific Markdown version of the 42 EvalHub "minishell" evaluation checklist. It mirrors the official checklist and highlights what to test locally for this repository. Where appropriate the wording was adjusted for clarity and to point to repository operations (build, run, relaunch, etc.).

---

## Attachments

- Official subject: https://cdn.intra.42.fr/pdf/pdf/179643/en.subject.pdf

---

## Quick notes for evaluators

- Work only from the student repository you cloned. Clone into an empty folder and verify the repo belongs to the evaluated student(s).
- If the repo is empty, contains cheating, or is non-functioning, use the available flags and follow EvalHub rules.
- Verify there are no malicious aliases or helper scripts that change behaviour; run tests and scripts in a clean environment when possible.
- Use `make -n` to check whether compilation includes `-Wall -Wextra -Werror`.

---

## How this checklist is laid out here

- Each section from the EvalHub page is reproduced below with short testing instructions and checkboxes you can mark during an evaluation run.
- Where EvalHub expects a flag (e.g. "crash", "incomplete work"), use your judgment and record your notes in the evaluation tool.

---

# Mandatory Part

### Compile
- [ ] `make -n` shows compilation with `-Wall -Wextra -Werror`.
- [ ] `make` completes without errors.
- [ ] The Makefile does not re-link unnecessarily.

### Simple Command & global variables
- [ ] Execute a simple command with an absolute path (e.g. `/bin/ls`).
- [ ] Empty command handled.
- [ ] Only allowed global variable used (signal number); ask student to justify any global usage.
- [ ] Inputs with only spaces/tabs handled gracefully.
- [ ] No crash during tests.

### Arguments
- [ ] Commands with arguments (no quotes) execute correctly.

### echo
- [ ] `echo` with and without `-n` behaves as expected.

### exit
- [ ] `exit` with/without arguments works and returns expected status. Relaunch minishell to continue tests.

### Return value of a process
- [ ] Commands return expected exit codes; verify via `echo $?` and compare with bash behaviour.

### Signals
- [ ] Ctrl-C on empty prompt: new line + new prompt, status 130.
- [ ] Ctrl-\ on empty prompt: no effect.
- [ ] Ctrl-D on empty prompt: exit minishell (relaunch to continue testing).
- [ ] Ctrl-C while typing: new line + new prompt and cleared buffer.
- [ ] Ctrl-D while typing: no action (buffer stays).
- [ ] Signals during blocking commands (e.g., `cat`) behave appropriately.

### Double Quotes
- [ ] Arguments inside double quotes preserve spaces and allow `$` expansion (except when escaped as per spec). Example: `echo "cat lol.c | cat > lol.c"`.

### Single Quotes
- [ ] Single quotes prevent expansion: `echo '$USER'` prints `$USER` literally.

### env
- [ ] `env` prints the environment.

### export
- [ ] `export` creates and replaces environment variables; verify with `env`.

### unset
- [ ] `unset` removes variables; verify with `env`.

### cd
- [ ] `cd` changes directory properly; verify with `/bin/ls` in target directory.
- [ ] `cd .` and `cd ..` behave as expected.

### pwd
- [ ] `pwd` prints current working directory.

### Relative Path
- [ ] Executing via relative path (many `..`) works correctly.

### Environment path
- [ ] Executing commands without path uses `$PATH` search order.
- [ ] `unset PATH` makes commands fail.
- [ ] Resetting `PATH` to multiple directories searches left-to-right.

### Redirection
- [ ] Input (`<`) and output (`>`, `>>`) redirections work across commands.
- [ ] Heredoc (`<<`) operation works; note: it does not have to update history.

### Pipes
- [ ] Multiple pipes work (e.g., `cat file | grep bla | more`).
- [ ] Mixing pipes and redirection behaves like bash.

### Go Crazy and history
- [ ] Buffer cleaned after `Ctrl-C` while typing; nothing executes afterwards.
- [ ] History navigation works (Up/Down) when appropriate.
- [ ] Commands that do not exist print errors and do not crash the shell.
- [ ] Long commands and many arguments handled correctly.

### Environment variables
- [ ] `$VARIABLE` interpolation works.
- [ ] Double quotes allow interpolation: `echo "$USER"` prints the value.

---

# Bonus

> Evaluate bonus only if the mandatory part is fully and correctly implemented.

### And, Or
- [ ] `&&`, `||` and parenthesis behave like bash.

### Wildcard
- [ ] Wildcards expand in arguments as in bash.

### Surprise tests
- [ ] Combined quoting tests such as `echo "'\$USER'"` behave per spec.

---

## Suggested evaluator commands

```bash
# build
make -n
make

# run
./minishell

# relaunch when needed (after exit)
./minishell
```

## Repository-specific notes

- This repository includes `scripts/run_minishell_tester.sh` which runs widely used automated tests. Use it to accelerate evaluation but still perform manual interactive checks listed above.
- If you encounter a `_codeql_detected_source_root` symlink in the repo root, it's a CodeQL artifact and can be removed safely:

```bash
git rm --cached --force _codeql_detected_source_root && rm -f _codeql_detected_source_root
```

---

## Changes made versus the original EvalHub page

- Reformatted the checklist into Markdown with checkboxes for convenience.
- Added short testing hints and repository-specific commands (`make`, `./minishell`, `scripts/run_minishell_tester.sh`).
- Preserved the scope and intent of the official evaluation items; no evaluation rules were altered.

---

*Adapted on 2026-03-15.*

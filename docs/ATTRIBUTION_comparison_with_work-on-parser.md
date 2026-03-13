# Attribution: Current Branch vs work-on-parser

Comparison by **By:** header in each file.  
- **You** = thanh-ng (42 login)  
- **Teammate** = gkhavari  

Reference: `origin/work-on-parser` vs current branch (`feature/complete-shell`).

---

## For gkhavari: What I (thanh-ng) changed and why

I modified your files so the project compiles, passes the tester, and stays within the 42 allowed functions. Your **By:** line is still on every file below. Here is what changed and why.

**You can share Section 2 (table with “What I changed” and “Why”) with her** — it’s the part that explains each of your edits in her code.

---

## 1. Files by author (current branch, src/ + includes/ only)

### By gkhavari (teammate) — 17 files

| File | On work-on-parser? | You changed this file? |
|------|--------------------|------------------------|
| `includes/defines.h` | Yes (gkhavari) | No (not in diff) |
| `src/free_runtime.c` | Yes (gkhavari) | **Yes** |
| `src/free_shell.c` | Yes (gkhavari) | **Yes** |
| `src/init.c` | Yes (gkhavari) | **Yes** |
| `src/parser/add_token_to_cmd.c` | Yes (gkhavari) | **Yes** |
| `src/parser/argv_build.c` | Yes (gkhavari) | **Yes** |
| `src/parser/heredoc.c` | Yes (gkhavari) | **Yes** |
| `src/parser/parser_syntax_check.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/continuation.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/expansion.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/expansion_utils.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/tokenizer.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/tokenizer_handlers.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/tokenizer_ops.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/tokenizer_quotes.c` | Yes (gkhavari) | **Yes** |
| `src/tokenizer/tokenizer_utils.c` | Yes (gkhavari) | **Yes** |
| `src/utils/ft_arrdup.c` | Was `arrdup.c` (gkhavari) | Renamed + small fix |

### By thanh-ng (you) — 26 files

| File | On work-on-parser? | Note |
|------|--------------------|------|
| `includes/includes.h` | Yes (thanh-ng) | Modified |
| `includes/minishell.h` | Yes (thanh-ng) | Modified |
| `includes/prototypes.h` | Yes (thanh-ng) | Modified |
| `includes/structs.h` | Yes (thanh-ng) | Modified |
| `src/builtins/builtin_dispatcher.c` | Yes (thanh-ng) | Modified |
| `src/builtins/cd.c` | Yes (thanh-ng) | Modified |
| `src/builtins/echo.c` | Yes (thanh-ng) | Modified |
| `src/builtins/env.c` | Yes (thanh-ng) | Modified |
| `src/builtins/exit.c` | Yes (thanh-ng) | Modified |
| `src/builtins/export.c` | Yes (thanh-ng) | Modified |
| `src/builtins/export_print.c` | **New** | You added |
| `src/builtins/export_utils.c` | Yes (thanh-ng) | Modified |
| `src/builtins/pwd.c` | Yes (thanh-ng) | Modified |
| `src/builtins/unset.c` | Yes (thanh-ng) | Modified |
| `src/executor/executor.c` | **New** | Replaces old `src/executor.c` |
| `src/executor/executor_child.c` | **New** | You added |
| `src/executor/executor_external.c` | **New** | You added |
| `src/executor/executor_pipeline.c` | **New** | You added |
| `src/executor/executor_utils.c` | **New** | You added |
| `src/free_utils.c` | **New** | You added |
| `src/main.c` | Yes (thanh-ng) | Modified |
| `src/parser/heredoc_utils.c` | **New** | You added |
| `src/parser/parser.c` | Yes (thanh-ng) | Modified |
| `src/signals/signal_handler.c` | Yes (thanh-ng) | Modified |
| `src/signals/signal_utils.c` | **New** | You added |
| `src/tokenizer/tokenizer_utils2.c` | **New** | You added |
| `src/utils.c` | Yes (thanh-ng) | Modified |

---

## 2. Changes to gkhavari’s code — what and why (for teammate)

**Files that were By: gkhavari and I modified (your name stayed in the header).**

| File | What I changed | Why |
|------|----------------|-----|
| **`src/free_runtime.c`** | Moved `free_tokens` and `free_args` into `free_utils.c` (to fix duplicate symbols). Removed `free_all` and the local `free_envp` from this file; `free_all` now lives only in `free_shell.c`. In `free_commands` I kept `free_out_redirs`, and kept the cleanup for `heredoc_fd`, `input_file` (with `is_heredoc` + `unlink` when applicable), `output_file`, and `heredoc_delim`. | After merging, we had two definitions of `free_all`, `free_tokens`, and `free_args`; the linker failed. One definition of each is now in a single file so the project compiles. |
| **`src/free_shell.c`** | Added `reset_shell()` to clear tokens, commands, and input between loops (and set pointers to NULL). Left `free_all()` as is; it is now the only place that implements `free_all`. | So the main loop can reset state each iteration without duplicate symbols, and so we have one clear place for “free everything and exit”. |
| **`src/init.c`** | `init_shell` now duplicates `envp` with `ft_arrdup(envp)` instead of storing the pointer directly; we own the environment and can change it (e.g. `export`/`unset`). If `getcwd()` fails, we set `shell->cwd = ft_strdup("/")` instead of only `perror`. Moved `get_env_value` here (it was needed for init and is used elsewhere). | Subject requires a modifiable environment; we must not use the original `envp` pointer. Fallback cwd avoids NULL and keeps the shell usable. |
| **`src/parser/add_token_to_cmd.c`** | There were two different `add_token_to_command` implementations (one `void`, one `int`). I kept a single version that **returns `int`** (1 = one token consumed, 2 = two tokens for redir/heredoc) and uses your **out_redirs** list and **heredoc_delim** (no `output_file`/`append` or `process_heredoc` here). Removed the duplicate that used `set_redir_file` and `process_heredoc`. `add_word_to_cmd` is still static and used only here. | `parser.c` expects a return value to know how many tokens to skip. Having two definitions caused a compile error; one implementation that matches the executor (out_redirs, heredoc via pipe) keeps behavior consistent. |
| **`src/parser/argv_build.c`** | Only formatting / comment alignment and Updated line in the header. Logic unchanged. | Norm / consistency. |
| **`src/parser/heredoc.c`** | Heredoc input is read via a **pipe** (not a temp file); we set `cmd->heredoc_fd` to the read end. Added `is_heredoc(char *f)` that always returns 0, because `free_commands` still calls it and we don’t use temp files. Removed the version that wrote to a file; kept the loop that reads until delimiter and handles expansion. | Unify with the rest of the code (heredoc as pipe). The stub `is_heredoc` satisfies the linker; we don’t unlink anything for heredoc. |
| **`src/parser/parser_syntax_check.c`** | Kept your syntax checks; possibly small tweaks or formatting. | So syntax errors are still reported before execution. |
| **`src/tokenizer/continuation.c`** | Small changes (e.g. prompt building or error handling). | To work with the current main loop and readline. |
| **`src/tokenizer/expansion.c`** | Expansion logic extended: **tilde `~`** (e.g. `~` → `$HOME`) and safe handling when **PATH is unset** (no crash). Comments/formatting may be adjusted. | Subject-like behavior and robustness; tilde is common in shells. |
| **`src/tokenizer/expansion_utils.c`** | Replaced `realloc` with **`ft_realloc`** (allowed functions). Replaced **`isspace()`** with explicit checks for space and tab (`' '`, `'\t'`) so we don’t use a disallowed function. | 42 allowed list: no `realloc`, no `isspace`; we use our own or only allowed libc. |
| **`src/tokenizer/tokenizer.c`** | Introduced `handle_quotes_and_expand()` that runs quote handling, single/double quote, **variable expansion**, and **tilde expansion** only when **not in heredoc mode** (`!is_heredoc_mode()`). Simplified the main loop to call this helper. | So we don’t expand inside heredoc when the delimiter is quoted (e.g. `<< 'EOF'`), and the loop is easier to follow. |
| **`src/tokenizer/tokenizer_handlers.c`** | In **`process_quote`**, when entering single or double quote, we now call **`mark_word_quoted()`** so the token gets the quoted flag. In **`handle_operator`**, when we see `<<`, we call **`set_heredoc_mode(1)`**. Replaced **`isspace()`** with `' '` / `'\t'` checks. | Quoted tokens are marked for heredoc (no expansion). Heredoc mode is set so the tokenizer knows not to expand in the heredoc body. Allowed functions only. |
| **`src/tokenizer/tokenizer_quotes.c`** | In double-quote handling, we only do **`$` expansion** when **not in heredoc mode** and when the next character is not `"` or `'` (so we don’t expand meaningless `$"` etc.). | Prevents expansion inside heredoc and avoids expanding invalid `$"` / `$'`. |
| **`src/tokenizer/tokenizer_utils.c`** | Added **`g_word_quoted`** and **`g_heredoc_mode`** (and `mark_word_quoted`, `set_heredoc_mode`, `is_heredoc_mode`) so we can skip expansion in heredoc and mark quoted words. **`flush_word`** now sets **`tok->quoted = g_word_quoted`** and resets the flag. **Removed the duplicate `add_token`** from this file; the only `add_token` is now in **`tokenizer_utils2.c`** (to fix duplicate symbol at link). | One definition of `add_token` in the project; heredoc and quoted tracking needed for correct behavior. |
| **`src/utils/ft_arrdup.c`** | File was **`arrdup.c`** on work-on-parser; renamed to **`ft_arrdup.c`** in the build. Replaced **`strdup(envp[i])`** with **`ft_strdup(envp[i])`**. | 42 allowed functions: `strdup` is not allowed; we use our `ft_strdup`. (We also have `ft_arrdup` in `utils.c`; the Makefile uses one of them to avoid duplicate symbols.) |

So: **15 files** that were **By: gkhavari** on work-on-parser were **edited by me (thanh-ng)**; the **By: gkhavari** line was **left in place** in all of them.

---

## 3. How much you changed teammate (gkhavari) code (summary)

**Same list as above, in short form:**

| File | Summary of change |
|------|--------------------|
| `src/free_runtime.c` | Removed `free_all`/duplicate; moved `free_tokens`/`free_args` to free_utils; kept `free_commands` and `free_out_redirs`. |
| `src/free_shell.c` | Added `reset_shell()`; now the only `free_all`. |
| `src/init.c` | `ft_arrdup(envp)`, cwd fallback to `"/"`, `get_env_value` moved here. |
| `src/parser/add_token_to_cmd.c` | Single `add_token_to_command` (int return, out_redirs/heredoc_delim); duplicate removed. |
| `src/parser/argv_build.c` | Formatting/header only. |
| `src/parser/heredoc.c` | Heredoc via pipe; `is_heredoc` stub added. |
| `src/parser/parser_syntax_check.c` | Syntax checks kept/refined. |
| `src/tokenizer/continuation.c` | Small changes. |
| `src/tokenizer/expansion.c` | Tilde expansion, safe missing PATH. |
| `src/tokenizer/expansion_utils.c` | `ft_realloc` instead of `realloc`; no `isspace`. |
| `src/tokenizer/tokenizer.c` | `handle_quotes_and_expand`, no expand in heredoc. |
| `src/tokenizer/tokenizer_handlers.c` | `mark_word_quoted`, `set_heredoc_mode`, no `isspace`. |
| `src/tokenizer/tokenizer_quotes.c` | Expand `$` only when not heredoc and next char not `"`/`'`. |
| `src/tokenizer/tokenizer_utils.c` | Heredoc/quoted globals; `flush_word` sets `quoted`; duplicate `add_token` removed. |
| `src/utils/ft_arrdup.c` | Renamed from arrdup.c; `strdup` → `ft_strdup`. |

---

## 4. Removed on your branch (existed on work-on-parser)

| File | By on work-on-parser |
|------|------------------------|
| `src/executor.c` | thanh-ng (you) — replaced by executor/* split. |
| `src/utils/simple_split.c` | thanh-ng (you) — removed from build. |

---

## 5. Summary

- **Teammate (gkhavari)** is still **By:** in **17** files in `src/` + `includes/`. You changed **15** of those files (plus the rename `arrdup.c` → `ft_arrdup.c`) but kept his name in the header.
- **You (thanh-ng)** are **By:** in **26** files. Of those, **7** are new (export_print, executor/*, free_utils, heredoc_utils, signal_utils, tokenizer_utils2); the rest existed on work-on-parser with you as author and you modified them.
- So: **you changed a lot of teammate’s (gkhavari) code** in parser, tokenizer, free, init, and utils, while **keeping his By: line**; and you added new files and refactored executor under your (thanh-ng) name.

To regenerate stats:

```bash
git diff origin/work-on-parser...HEAD --stat
grep -rl "By: gkhavari" src includes
grep -rl "By: thanh-ng" src includes
```

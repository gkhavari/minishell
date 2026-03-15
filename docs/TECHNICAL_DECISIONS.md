# Technical Decisions

This document records **what we changed and why** in the minishell codebase: data structures, function design, defensive behavior, and 42 constraints. Use it for onboarding, code review, and avoiding regressions. When making a significant change, add a short entry here.

---

## 1. Data model & structures


| Decision                            | What we did                                                                                                           | Why                                                                                                                                                                                                                           |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Own the environment**             | `init_shell` duplicates `envp` with `ft_arrdup(envp)` instead of storing the caller’s pointer.                        | Subject requires a modifiable environment; `export`/`unset`/`cd` must change vars without touching the original `envp`.                                                                                                       |
| **Single redirection list**         | Each command has one `t_redir *redirs` with `is_input` and `append` flags (no separate `input_file` / `output_file`). | Multiple redirections per command; order matters (left-to-right). One list keeps `apply_redirections()` simple and matches “sequence of redir ops” semantics. See [DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md). |
| **Heredoc via pipe, not temp file** | Heredoc content is written to a **pipe**; `cmd->heredoc_fd` holds the read end. No temp file or `unlink`.             | Avoids file system, race conditions, and cleanup; executor just `dup2(heredoc_fd, STDIN_FILENO)`.                                                                                                                             |
| **Args then argv**                  | Commands store `t_arg *args` during parse and build `char **argv` in `finalize_argv()`.                               | Parsing appends args one-by-one; execution needs a NULL-terminated array. One conversion step keeps parse and execution clearly separated.                                                                                    |
| **CWD fallback**                    | If `getcwd(NULL, 0)` fails in `init_shell`, we set `shell->cwd = ft_strdup("/")`.                                     | Avoids NULL `cwd`; prompt and `cd` logic stay safe.                                                                                                                                                                           |
| **Quoted flag on tokens**           | `t_token` has `int quoted`; set in `flush_word()` from `mark_word_quoted()`.                                          | Heredoc: quoted delimiter → no expansion in body. Parser/executor can treat quoted args differently if needed.                                                                                                                |


---

## 2. Functions & APIs


| Decision                                     | What we did                                                                                                                                                                   | Why                                                                                                                         |
| -------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `**add_token_to_command` returns int**       | Returns `1` (one token consumed) or `2` (two tokens for redir/heredoc).                                                                                                       | Parser must know how many tokens to advance; a single return contract avoids two competing implementations and link errors. |
| **Single definition of free helpers**        | `free_tokens` and `free_args` live in `free/free_utils.c`; `free_all` only in `free/free_shell.c`. `free_commands` stays in `free/free_runtime.c` and calls `free_args`.                     | After merge we had duplicate symbols; one definition per symbol is required for a clean link.                               |
| `**reset_shell` per loop**                   | `reset_shell()` frees tokens, commands, input and sets pointers to NULL. Called after each line in the main loop.                                                             | Prevents leaks between lines and gives a clear “per-iteration” boundary; `free_all` remains “teardown on exit” only.        |
| `**get_env_value` in core/init.c**                | Lookup for `KEY=value` in `envp` lives in `core/init.c` and is used from builtins, executor, expansion, heredoc.                                                                   | Single place for env lookup; init needs it first, others reuse.                                                             |
| **Heredoc: no expand when delimiter quoted** | In tokenizer, `set_heredoc_mode(1)` when we see `<<`; expansion (variable, tilde) is skipped when `is_heredoc_mode()`. In parser, `cmd->heredoc_quoted` from delimiter token. | Bash: `<< 'EOF'` or `<< "EOF"` means no expansion in the body; we match that.                                               |
| `**is_heredoc` stub in heredoc.c**           | Legacy `is_heredoc(char *f)` kept as stub (returns 0) where still referenced.                                                                                                 | We don’t use temp-file heredoc; stub satisfies existing call sites without rewriting all callers at once.                   |
| **Builtin exit: non-numeric → 255**          | For `exit <non-numeric>`, we print “numeric argument required” to stderr and **exit(255)**.                                                                                   | Bash (3.2 and 5.x) uses 255 for this case; testers and scripts expect 255, not 2.                                           |


---

## 3. Defensive & bug prevention


| Decision                                              | What we did                                                                                                | Why                                                                                               |
| ----------------------------------------------------- | ---------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| **No expansion inside heredoc when delimiter quoted** | In double-quote handling we expand `$` only when not in heredoc mode and when next char is not `"` or `'`. | Prevents expanding inside `<< 'EOF'` and avoids meaningless `$"` / `$'` expansion.                |
| **Tilde expansion only when not heredoc**             | `handle_tilde_expansion` is called only when `!is_heredoc_mode()`.                                         | Same as variable expansion: heredoc with quoted delimiter must not expand.                        |
| **Safe missing PATH**                                 | When PATH is unset or missing, path search does not crash; we report “command not found” and exit 127.     | Robustness for minimal or corrupted environments.                                                 |
| **Allowed functions only (realloc, isspace)**         | Replaced `realloc` with `ft_realloc`; replaced `isspace()` with explicit `' '` and `'\t'` checks.          | 42 allowed list: no `realloc`, no `isspace`; we use our own or allowed libc only.                 |
| **Allowed functions only (strdup)**                   | In env duplication we use `ft_strdup(envp[i])`, not `strdup`.                                              | 42: `strdup` is not allowed.                                                                      |
| **One `add_token` in the project**                    | Only one definition of `add_token` (in `tokenizer_utils2.c`); removed duplicate from tokenizer_utils.      | Duplicate symbol at link; one implementation avoids confusion and link errors.                    |
| **Quote state and heredoc mode**                      | `mark_word_quoted()` when entering single/double quote; `set_heredoc_mode(1)` when we see `<<`.            | Ensures tokens and heredoc body are marked correctly for “no expansion” and for future semantics. |


---

## 4. 42 / Norm constraints


| Decision                          | What we did                                                                                  | Why                                                                                                                                        |
| --------------------------------- | -------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| **Single global variable**        | Only `g_signum` (volatile sig_atomic_t) is global; set in signal handler, read in main loop. | Subject allows one global; we use it only for the signal number so we can react after readline returns.                                    |
| **No strdup / realloc / isspace** | Use `ft_strdup`, `ft_realloc`, and explicit space/tab checks.                                | Stay within the 42 allowed function list.                                                                                                  |
| **ft_arrdup**                     | `ft_arrdup` lives in `utils/utils.c` only (with `ft_strcat`, `ft_realloc`, `msh_calloc`).   | Single definition; no separate ft_arrdup.c to avoid duplicate symbol. |


---

## 5. File layout & merge resolutions

These decisions came from merging branches and avoiding duplicate or conflicting code.


| Area          | Decision                                                                                                                                                 | Why                                                                                                                              |
| ------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| **Executor**  | Split into `executor/executor.c`, `executor/executor_utils.c`, `executor/executor_external.c`, `executor/executor_child.c`, `executor/executor_pipeline.c`.                                   | Clear separation: entry point, redirections, external vs builtin, child process, pipeline. All under src/executor/.               |
| **Free**      | `free/free_utils.c`: `free_tokens`, `free_args`. `free/free_runtime.c`: `free_commands` only. `free/free_shell.c`: `reset_shell`, `free_all`, and internal `free_envp`. | No duplicate symbols; one place per responsibility.                                                                           |
| **Tokenizer** | `add_token` and `new_token` in `tokenizer/tokenizer_utils2.c`; `flush_word` and quoted/heredoc flags in `tokenizer/tokenizer_utils.c`.                   | Single definition of `add_token`; token creation and word flushing stay organized. All under src/tokenizer/.                    |
| **Parser**    | Single `add_token_to_command` with int return and `redirs`/`heredoc_delim`; no second implementation using different redir fields.                       | Parser and executor share the same command/redir model; one implementation avoids drift and link errors.                         |


---

## 6. How to use this doc

- **Onboarding:** Read §1–4 to see why structures and APIs are as they are.
- **Before changing free/executor/tokenizer/parser:** Check §2 and §5 so you don’t reintroduce duplicate symbols or break the single-definition choices.
- **When adding a feature:** Prefer existing patterns (e.g. one list for redirs, return value from `add_token_to_command`).
- **When fixing a bug:** Add a one-line entry under §2 or §3 (what we did, why) so the team knows the rationale.
- **42 / Norm:** Before using a new libc function, check §4 and the allowed list.

---

## Related docs


| Document                                                   | Content                                           |
| ---------------------------------------------------------- | ------------------------------------------------- |
| [DATA_MODEL_AND_FUNCTIONS.md](DATA_MODEL_AND_FUNCTIONS.md) | Data model rationale and full function reference. |
| [minishell_architecture.md](minishell_architecture.md)     | Pipeline, signals, source layout.                 |
| [BEHAVIOR.md](BEHAVIOR.md)                                 | Input/output semantics and exit codes.            |



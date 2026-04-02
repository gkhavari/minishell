# Tokenizer & Parser Design Notes (for defense)

This document explains how input becomes executable command structures:

1) lexer/tokenizer: raw line -> `t_token` list
2) parser: `t_token` list -> `t_command` pipeline
3) argv finalization: `args` list -> `argv[]`

Goal: help teammates understand behavior, error handling, and constraints.

---

## 1. High-level flow

For non-empty input lines, the core path is:

- `tokenize_input(shell)` in `src/tokenizer/tokenizer.c`
- `parse_input(shell)` in `src/parser/parser.c`
- `process_heredocs(shell)` in `src/parser/parser.c`
- `run_commands(shell)` in `src/executor/exe.c`

Tokenizer and parser are intentionally separated:

- tokenizer knows chars, quote state, operators, expansion
- parser knows shell grammar pieces and command data model

---

## 2. Tokenizer design

### 2.1 Tokenizer state machine

Tokenizer uses `t_state`:

- `ST_NORMAL`
- `ST_SQUOTE`
- `ST_DQUOTE`

Main loop lives in `src/tokenizer/tokenizer_loop.c`:

- while current char exists:
  - quote/open-close handling
  - quoted-segment handlers
  - unquoted expansion handlers
  - backslash handling
  - operator handling (`|`, `<`, `>`, `>>`, `<<`)
  - whitespace flush
  - normal char append

The loop returns `OOM` only for allocation failures.

### 2.2 Word accumulation and flush

Tokenizer builds the current word in a temporary `char *word`.

- chars are appended with helpers (`append_char`, etc.)
- on operator/whitespace/end, `flush_word(...)` emits `WORD` token
- operator handlers emit operator tokens directly

This keeps token emission centralized and avoids partial-token bugs.

### 2.3 Quote behavior

Quote transitions are handled in `process_quote(...)`:

- entering `'` or `"` sets quote state and marks word as quoted
- closing quote returns to `ST_NORMAL`

If end-of-line is reached while still quoted:

- tokenizer prints `minishell: syntax error: unclosed quote`
- sets `last_exit` to syntax code (`XSYN`)
- clears temporary word and tokens for that line

Implementation: `handle_end_of_string(...)` in
`src/tokenizer/tokenizer_handlers.c`.

### 2.4 Expansion behavior at lexer stage

Expansion is integrated into tokenization:

- unquoted path calls variable and tilde handlers
- heredoc mode disables normal variable expansion in unquoted path

`expand_var(...)` in `src/tokenizer/expansion.c` handles:

- `$?` -> last exit as string
- `$<digit>` -> empty
- `$'` / `$"` special handling
- `$NAME` and `$_` style via env lookup
- lone `$` or non-var char after `$` -> literal `$`

### 2.5 Operator and heredoc marker behavior

`handle_operator(...)`:

- flushes current word first
- reads operator token length (`<`, `>`, `|`, `<<`, `>>`)
- sets `shell->heredoc_mode = 1` when `<<` is tokenized

This flag is part of expansion policy around heredoc handling.

---

## 3. Parser design

### 3.1 Syntax gate first

`parse_input(shell)` starts with `syntax_check(tokens)`:

- rejects leading `|`
- rejects `||`-like adjacency of `PIPE` tokens
- enforces "redir must be followed by WORD"

On syntax error:

- prints bash-style message from `syntax_error(...)`
- sets `last_exit = XSYN`
- frees token list and returns

Implementation: `src/parser/parser_syntax_check.c`.

### 3.2 Build command pipeline structure

`build_command_list(...)` in `src/parser/parser_build.c` walks tokens and
constructs a `t_list` of `t_command`.

- `PIPE` token => append a new empty `t_command`
- non-pipe token => `add_token_to_command(...)`

`add_token_to_command(...)` in `src/parser/add_token_to_cmd.c` maps tokens:

- `WORD` -> append to `cmd->args`
- `HEREDOC` -> store delimiter metadata in command
- redir tokens (`<`, `>`, `>>`) -> parse token pair into `cmd->redirs`

### 3.3 Redirection parse contract

`parse_redir_token_pair(...)` in `src/parser/parser_redir.c` requires:

- current token is redirection
- next token exists and is `WORD`

Then it appends a `t_redir` entry with:

- `file`
- destination fd (`STDIN_FILENO` or `STDOUT_FILENO`)
- append flag for `>>`

Returns sentinel status (`PR_2`, `PR_1`, `PR_ERR`, `OOM`) so caller advances
the token cursor correctly and can unwind robustly.

### 3.4 Arg list to argv[] finalization

After command list build, parser finalizes each command:

- `finalize_all_commands(...)` in `src/parser/argv_build.c`
- allocates `cmd->argv` and duplicates all `cmd->args` values
- sets `cmd->is_builtin` via `get_builtin_type(...)`
- special case: `env` with arguments is not treated as shell builtin here

This separation keeps parse structure independent from execution array format.

---

## 4. Error handling strategy (important for defense)

### 4.1 OOM propagation

Tokenizer/parser helpers return `OOM` on allocation failure instead of exiting
the process.

Upper layers own unwind:

- `tokenize_input` uses `free_lex(...)` on OOM
- parser frees partial command list on OOM and marks shell OOM path

This prevents partial object leaks and keeps one clear owner per phase.

### 4.2 Syntax failures are line-local

When syntax fails:

- the current input line is rejected
- token/parse intermediates are cleared
- shell state is kept valid for next prompt iteration

### 4.3 Non-interactive behavior

In non-interactive mode (`minishell < file`), syntax errors can stop further
processing for remaining input stream in current design path. Interactive mode
stays at prompt and continues.

---

## 5. Why this design is good

- **Separation of concerns**: tokenizer handles chars/quotes/expansion;
  parser handles grammar and command model.
- **Small-step contracts**: helper functions return explicit status sentinels.
- **Memory safety**: OOM routes are explicit and centralized.
- **Testability**: syntax, expansion, and parser logic are isolated enough
  for targeted regression runs.
- **Executor-friendly output**: parser outputs clean `t_command` objects with
  `argv`, `redirs`, heredoc metadata, and builtin hint.

---

## 6. What we intentionally do not parse/implement

Not in current mandatory scope:

- control operators: `&&`, `||`, `;`
- job control: `&`, `jobs`, `fg`, `bg`
- advanced fd syntax: `2>`, `2>>`, `2>&1`, `n>`, `<&`, `>&`
- grouping/subshell forms: `(...)`, `{ ...; }`

Tokenizer/parser are intentionally shaped around mandatory minishell grammar.

---

## 7. Known constraints and trade-offs

- Expansion is integrated during lexical pass, which simplifies parser but
  means some policy is lexer-coupled (by design).
- Heredoc behavior depends on per-command delimiter metadata and heredoc mode
  flag; this keeps implementation compact but requires careful sequencing.
- Sentinel-based control flow (`OOM`, `PR_ERR`, `PR_1`, `PR_2`) improves Norm-
  friendly small functions but can be less obvious without documentation.

---

## 8. Quick defense mental model

1. Lexer walks chars with quote state and emits typed tokens.
2. Parser validates token grammar before building commands.
3. Command builder maps tokens into `args`, `redirs`, heredoc metadata.
4. Finalizer converts `args` to `argv[]` and marks builtin type.
5. Any syntax/OOM issue is contained and unwound without corrupting REPL state.


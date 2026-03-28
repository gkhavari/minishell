# The Norm (Version 4.1)

Summary: the Norm is the coding standard used at 42 for C projects. It defines naming, formatting,
and structural rules that help keep code consistent, readable, and maintainable. When relevant,
rules that are not automatically validated by the norminette are marked with `(*)`.

Repository: https://github.com/42School/norminette

--

## Foreword

The `norminette` tool checks many items from the Norm, but not all (some rules remain subjective).
When evaluating, the norminette verdict is authoritative for automated checks; manual review covers
the remaining items. Rules marked `(*)` require special attention during manual review and can
cause a Norm failure if violated.

--

## Rationale

The Norm exists for pedagogical and practical reasons:

- Sequencing: prefer small functions (max 25 logical lines) that do one clear task. This simplifies
    reasoning and debugging and avoids complex, error-prone code constructs.
- Look & feel: a shared style makes peer review faster and keeps repositories readable across the
    cohort.
- Long-term maintenance: readable code reduces the cost of fixes and feature additions.

See the Norm rules below for concrete constraints and examples.

--

## The Norm (summary)

### III.1 Naming

- Structures: prefix `s_`
- Typedefs: prefix `t_`
- Unions: prefix `u_`
- Enums: prefix `e_`
- Globals: prefix `g_`
- Identifiers (functions, types, variables): lowercase, digits and `_` only (snake_case). No caps.
- Filenames and directories: lowercase, digits and `_` only.
- Non-ASCII characters are forbidden outside string/char literals.
- `(*)` Identifiers should be explicit and English-readable; avoid single-letter names.
- Globals (non-const, non-static) are forbidden unless a project explicitly allows them.

All source files must compile.

### III.2 Formatting

- Max 25 lines per function (excluding braces).
- Max 80 columns per line (comments included).
- Functions separated by a single empty line; comments/preprocessor directives may appear between functions.
- Indentation: use a 1-tab character (visual width 4). Configure your editor to use real tabs.
- Braces: opening/closing braces on their own line (except in declarations of struct/enum/union).
- Empty lines must contain no whitespace. No two consecutive empty lines.
- No trailing spaces or tabs at line ends.
- Declarations must appear at the start of a function; one variable declaration per line.
- Pointer asterisks (`*`) must be attached to the variable name.
- One instruction or control structure per line.
- When splitting a long expression, indent continuation lines and place operators at the start of the new line.
- Use a single space after commas and semicolons (unless end of line).
- Operators and operands: single spaces around binary operators.
- Control structures must use braces, except for a single-line body.

### III.3 Functions

- Max 4 named parameters.
- Functions with no parameters must use `void` in the prototype.
- Prototype parameters must be named.
- Max 5 local variable declarations per function.
- Return statements should use parentheses when returning a value (e.g. `return (value);`).
- One tab between return type and function name.

Example:

```c
int	my_func(int arg1, char arg2, char *arg3)
{
	return (my_val);
}

int	func2(void)
{
	return ;
}
```

### III.4 Typedef, struct, enum and union

- Add a space after `struct`, `enum`, `union` and `typedef` when declaring names.
- Indent members inside braces following normal block rules.
- Do not define structures in `.c` files (declare them in headers).

### III.5 Headers

Allowed items: includes, declarations, defines, prototypes and macros.

- All `#include` directives must be at the top of the file.
- Never `#include` a `.c` file.
- Protect headers against multiple inclusion (e.g. `FT_FOO_H`).
- `(*)` Avoid including unused headers; justify exceptions with a comment.

### III.6 The 42 header

Every `.c` and `.h` file must begin with the standard 42 file header that includes author,
creation date and last update. The header should be kept up-to-date by the editor/cluster tools.

### III.7 Macros and preprocessor

- `#define` constants must be used for literal/constant values only.
- `#define` used to bypass the Norm or obfuscate code is forbidden.
- Do not use multiline macros.
- Macro names must be UPPERCASE.
- Indent preprocessor directives inside conditional blocks.
- Preprocessor directives are only allowed in global scope.

### III.8 Forbidden constructs

The following are not allowed:

- `for`
- `do ... while`
- `switch` / `case`
- `goto`
- Ternary operator (`?:`)
- Variable Length Arrays (VLAs)
- Implicit type in declarations

### III.9 Comments

- Comments must be outside function bodies or on their own line; end-of-line comments are allowed.
- `(*)` Comments should be in English and provide useful information.
- Comments do not justify poor design or overly complex functions.

### III.10 Files

- Do not `#include` `.c` files.
- Max 5 function definitions per `.c` file.

### III.11 Makefile

- The targets `$(NAME)`, `all`, `clean`, `fclean`, and `re` are mandatory.
- `all` must be the default target.
- The Makefile must not relink unnecessarily.
- For projects using a local library (e.g. `libft`), the Makefile must build it automatically.
- List every source file explicitly (no `*.c` or `*.o` globs).

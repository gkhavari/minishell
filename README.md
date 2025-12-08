# minishell

minishell/
 ├── src/
 │   ├── main.c
 │   ├── signals/
 │   ├── parser/
 │   ├── executor/
 │   ├── builtins/
 ├── includes/
 ├── Makefile

# Minishell Project Analysis & Requirements

## 1. Project Overview
The goal is to write a simple shell (command-line interpreter) based on Bash. It involves extensive management of processes and file descriptors.

## 2. Core Constraints
* **Language:** C
* **Norm:** Must strictly follow the Norm (including bonus files).
* **Stability:** No unexpected crashes (segfault, bus error, double free). Any crash = 0.
* **Memory:** No memory leaks allowed.
    * *Exception:* `readline()` may cause reachable leaks you don't need to fix.
* **Global Variable:** Maximum **ONE** global variable allowed.
    * **Strict Usage:** It can *only* store the received signal number.
    * It cannot contain complex data structures.
## 3. Mandatory Features

### A. Loop & Interface
* Display a prompt when waiting for a new command.
* Maintain a working history.
* Do not interpret unclosed quotes or special characters like `\` or `;`.

### B. Parsing & Expansion
* **Quotes:**
    * Single Quotes (`'`): Prevent interpretation of *all* meta-characters.
    * Double Quotes (`"`): Prevent interpretation of meta-characters *except* `$` (dollar sign).
* **Environment Variables:** Handle `$` followed by characters to expand values.
* **Exit Status:** Handle `$?` to expand to the exit status of the most recent foreground pipeline.

### C. Execution & Redirection
* **Search:** Launch executables via PATH variable, relative path, or absolute path.
* **Redirections:**
    * `<`: Redirect input.
    * `>`: Redirect output.
    * `<<`: Heredoc (read input until delimiter). Does not update history.
    * `>>`: Append mode output.
* **Pipes:** Implement `|`. Connect output of command A to input of command B.

### D. Signals
* Behave like Bash for:
    * `Ctrl-C`: Display new prompt on a new line.
    * `Ctrl-D`: Exit the shell.
    * `Ctrl-\`: Do nothing.

### E. Built-in Commands
You must implement the following built-in commands:
1.  `echo` (with `-n` option).
2.  `cd` (relative or absolute path only).
3.  `pwd` (no options).
4.  `export` (no options).
5.  `unset` (no options).
6.  `env` (no options/args).
7.  `exit` (no options).

## 4. Edge Cases & Tricky Areas
* **Signal Safety:** Since you can only use one global variable for the signal number, your main loop needs to check this variable frequently to react (e.g., stopping a blocking `read`).
* **Pipes & Forks:** Ensure all file descriptors are closed correctly. If one pipe end remains open, `waitpid` might hang.
* **Heredoc:** Must handle the delimiter correctly and not save the input to history.
* **Quote Mixing:** Handling strings like `'echo "' hello` correctly (the double quote is inside single quotes, so it is a literal).
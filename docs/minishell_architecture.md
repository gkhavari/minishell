# Minishell Project Architecture (Defensive Programming)

> **Philosophy:** Defensive programming means we validate every input, handle every error case explicitly, and never assume success. We use bash as our reference implementation but only implement what the 42 subject requires.

---

## 1. Global State & Signal Handling

### 1.1 The Global Variable Rule

```c
/* ONLY global variable allowed in the entire project */
volatile sig_atomic_t	g_signum = 0;
```

| Value        | Meaning         | Action Required                                    |
| ------------ | --------------- | -------------------------------------------------- |
| `0`          | No signal       | Continue normally                                  |
| `SIGINT (2)` | Ctrl+C received | Print newline, new prompt, set `exit_status = 130` |

**Critical Rules:**

- ❌ Do NOT store structs, pointers, or flags in global
- ❌ Do NOT access `t_shell` from signal handler
- ✅ Only store the signal NUMBER
- ✅ Check `g_signum` in main loop AFTER `readline()` returns

**Why `volatile sig_atomic_t`?**

- `volatile`: Tells compiler the value can change at any time (by signal handler)
- `sig_atomic_t`: Guaranteed atomic read/write (no partial updates)

### 1.2 Signal Behavior (Bash Reference)

| Signal             | Interactive Mode     | During Execution                  | During Heredoc                     |
| ------------------ | -------------------- | --------------------------------- | ---------------------------------- |
| `SIGINT` (Ctrl+C)  | New prompt, `$?=130` | Kill child, new prompt            | Stop heredoc, new prompt, `$?=130` |
| `SIGQUIT` (Ctrl+\) | Ignored              | Kill child + "Quit (core dumped)" | Ignored                            |
| `EOF` (Ctrl+D)     | Exit shell           | N/A (not a signal)                | Close heredoc input                |

**Implementation Pattern:**

```c
/* In main loop, AFTER readline returns */
if (g_signum == SIGINT)
{
    shell->last_exit = 130;
    g_signum = 0;  /* Reset for next iteration */
}
```

### 1.3 Defensive Initialization Checklist

```
init_shell(t_shell *shell, char **envp)
├── 1. Zero the struct: ft_bzero(shell, sizeof(t_shell))
├── 2. Duplicate envp
│   ├── IF envp == NULL || envp[0] == NULL
│   │   └── Create minimal env: PWD, SHLVL=1, _=/usr/bin/env
│   └── ELSE
│       └── ft_arrdup(envp) + increment SHLVL
├── 3. Set shell->cwd = getcwd(NULL, 0)
│   └── IF getcwd fails → use getenv("PWD") or "/"
├── 4. Set shell->user from getenv("USER") or "user"
├── 5. Set shell->last_exit = 0
└── 6. Setup signal handlers (sigaction preferred over signal)
```

**Defensive Check - SHLVL:**

```c
/* Bash behavior: SHLVL starts at 1, increments for nested shells */
char *shlvl = get_env_value(shell, "SHLVL");
int level = (shlvl) ? ft_atoi(shlvl) + 1 : 1;
if (level < 0)
    level = 0;  /* Defensive: handle negative values */
if (level > 1000)
    level = 1;  /* Bash resets at 1000 */
```

---

## 2. Main Loop (REPL Cycle)

```
┌─────────────────────────────────────────────────────────────┐
│                     MAIN LOOP START                         │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  1. CHECK g_signum                                          │
│     └── If SIGINT: set last_exit=130, reset global          │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  2. BUILD & DISPLAY PROMPT                                  │
│     └── [exit_status_color] user@cwd $                      │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  3. READLINE (blocking call)                                │
│     ├── Returns NULL: Ctrl+D pressed → print "exit", break  │
│     ├── Returns "": Empty input → continue (skip processing)│
│     └── Returns string: Process input                       │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  4. CHECK g_signum AGAIN                                    │
│     └── Ctrl+C during readline: skip processing, continue   │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  5. ADD TO HISTORY (if non-empty input)                     │
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  6. LEXER → PARSER → EXPANDER → EXECUTOR                    │
│     └─ Any stage can fail → set last_exit, cleanup, continue│
└─────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────┐
│  7. CLEANUP (free tokens, commands, reset for next loop)    │
└─────────────────────────────────────────────────────────────┘
         │
         └──────────────────► LOOP BACK TO START
```

---

## 3. Lexer (Tokenization)

### 3.1 Token Types (Matching Your structs.h)

```c
typedef enum e_tokentype
{
    WORD,       /* Commands, arguments, filenames */
    PIPE,       /* | */
    REDIR_IN,   /* < */
    REDIR_OUT,  /* > */
    APPEND,     /* >> */
    HEREDOC     /* << */
}   t_tokentype;
```

### 3.2 Lexer State Machine

```
Input: echo "hello world" | cat < file.txt

State: NORMAL
        │
        ├── Whitespace → Skip
        ├── Quote (' or ") → Enter QUOTED state
        ├── | → Emit PIPE token
        ├── < → Check next char
        │       ├── < → Emit HEREDOC
        │       └── else → Emit REDIR_IN
        ├── > → Check next char
        │       ├── > → Emit APPEND
        │       └── else → Emit REDIR_OUT
        └── Other → Accumulate into WORD

State: SINGLE_QUOTED (')
        └── Everything is literal until closing '

State: DOUBLE_QUOTED (")
        ├── $ → Mark for expansion (but still in WORD)
        └── Everything else literal until closing "
```

### 3.3 Syntax Error Detection (Defensive Checks)

**Error: Unclosed Quotes**

```bash
$ echo "hello        # bash: unexpected EOF while looking for matching `"'
$ echo 'hello        # bash: unexpected EOF while looking for matching `''
```

**Our behavior:** Print error, set `last_exit = 2`, do NOT execute.

**Error: Invalid Pipe Usage**

```bash
$ | ls              # bash: syntax error near unexpected token `|'
$ ls |              # bash: syntax error near unexpected token `newline'
$ ls || cat         # We don't handle || (logical OR) - treat as syntax error
$ ls | | cat        # bash: syntax error near unexpected token `|'
```

**Our behavior:** Print `minishell: syntax error near unexpected token`, set `last_exit = 2`.

**Error: Invalid Redirection**

```bash
$ ls >              # bash: syntax error near unexpected token `newline'
$ ls > > file       # bash: syntax error near unexpected token `>'
$ ls < >            # bash: syntax error near unexpected token `>'
```

### 3.4 Defensive Lexer Validation Function

```c
int	validate_syntax(t_token *tokens)
{
    t_token *curr = tokens;
    t_token *prev = NULL;

    while (curr)
    {
        /* Rule 1: Pipe cannot be first or last */
        if (curr->type == PIPE && (prev == NULL || curr->next == NULL))
            return (syntax_error("|"));

        /* Rule 2: Pipe cannot follow pipe */
        if (curr->type == PIPE && prev && prev->type == PIPE)
            return (syntax_error("|"));

        /* Rule 3: Redirection must be followed by WORD */
        if (is_redirection(curr->type) &&
            (curr->next == NULL || curr->next->type != WORD))
            return (syntax_error(get_token_str(curr->type)));

        /* Rule 4: Redirection cannot follow redirection directly */
        if (is_redirection(curr->type) && prev && is_redirection(prev->type))
            return (syntax_error(get_token_str(curr->type)));

        prev = curr;
        curr = curr->next;
    }
    return (0);  /* Valid */
}
```

---

## 4. Expansion (Variable Substitution)

### 4.1 Expansion Order (Critical!)

```
┌──────────────────────────────────────────────────────────────┐
│  STEP 1: Variable Expansion ($VAR, $?)                       │
│  ─────────────────────────────────────────────────────────── │
│  • Happens INSIDE double quotes: "$HOME" → "/home/user"      │
│  • Does NOT happen inside single quotes: '$HOME' → "$HOME"   │
│  • Unset variable → empty string: $UNDEFINED → ""            │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  STEP 2: Quote Removal                                       │
│  ─────────────────────────────────────────────────────────── │
│  • "hello" → hello                                           │
│  • 'world' → world                                           │
│  • "hello"'world' → helloworld (concatenation)               │
└──────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────────────────────────────────────────────────┐
│  STEP 3: Word Splitting (We DON'T implement this fully)      │
│  ─────────────────────────────────────────────────────────── │
│  • Bash splits unquoted expansions by IFS                    │
│  • We keep it simple: expanded value stays as ONE argument   │
│  • This is acceptable for 42 subject scope                   │
└──────────────────────────────────────────────────────────────┘
```

### 4.2 Variable Expansion Rules

| Input        | Context         | Result             | Explanation                |
| ------------ | --------------- | ------------------ | -------------------------- |
| `$HOME`      | Unquoted        | `/home/user`       | Normal expansion           |
| `"$HOME"`    | Double quotes   | `/home/user`       | Expansion works in ""      |
| `'$HOME'`    | Single quotes   | `$HOME`            | NO expansion in ''         |
| `$?`         | Any (except '') | `0` (or last exit) | Exit status                |
| `$UNDEFINED` | Any             | `` (empty)         | Unset → empty string       |
| `$`          | End of word     | `$`                | Literal $ (no var name)    |
| `$123`       | Any             | `$123`             | Invalid var name → literal |
| `"$"`        | Double quotes   | `$`                | Lone $ is literal          |

### 4.3 Variable Name Rules

```c
/* Valid variable name: starts with letter or _, followed by alnum or _ */
int is_valid_var_char(char c, int is_first)
{
    if (is_first)
        return (ft_isalpha(c) || c == '_');
    return (ft_isalnum(c) || c == '_');
}
```

### 4.4 Defensive Expansion Examples

```bash
# Test cases to verify your expander:

echo $HOME              # /home/user
echo "$HOME"            # /home/user
echo '$HOME'            # $HOME
echo $?                 # 0 (or last exit code)
echo "$?"               # 0
echo '$?'               # $?
echo $UNDEFINED         # (empty line)
echo "$UNDEFINED"       # (empty line)
echo $USER$HOME         # userhome (concatenated)
echo "$USER$HOME"       # user/home/user
echo $                  # $
echo "hello$"           # hello$
echo $123               # $123 (invalid var name)
echo $USER_NAME         # (value of USER_NAME, not USER + _NAME)
```

---

## 5. Parser (Command Table Construction)

### 5.1 Command Structure

```c
typedef struct s_arg
{
    char            *value;
    struct s_arg    *next;
}   t_arg;

typedef struct s_redir
{
    char            *file;
    int             append;
    struct s_redir  *next;
}   t_redir;

typedef struct s_command
{
    t_arg               *args;          /* Linked list of args before finalizing */
    char                **argv;         /* ["ls", "-la", NULL] for execve */
    char                *input_file;    /* Filename for < */
    t_redir             *out_redirs;    /* Linked list of output redirections */
    int                 heredoc_fd;     /* FD for heredoc input (or -1) */
    char                *heredoc_delim; /* Delimiter for heredoc */
    int                 heredoc_quoted; /* Flag if delimiter was quoted */
    int                 is_builtin;     /* Enum or flag */
    struct s_command    *next;          /* Next command in pipeline */
}   t_command;
```

### 5.2 Parsing Flow

```
Tokens: [echo] [hello] [|] [cat] [<] [file.txt]
                    │
                    ▼
        ┌──────────────────────┐
        │   Split by PIPE      │
        └──────────────────────┘
                    │
        ┌───────────┴───────────┐
        ▼                       ▼
   Command 1               Command 2
   [echo] [hello]          [cat] [<] [file.txt]
        │                       │
        ▼                       ▼
   argv: ["echo",          argv: ["cat", NULL]
          "hello",         input_file: "file.txt"
          NULL]
```

### 5.3 Redirection Parsing (Right-to-Left for Multiple)

```bash
# Bash behavior: Last redirection wins
$ echo hello > file1 > file2    # Creates both, writes to file2
$ cat < file1 < file2           # Opens both, reads from file2

# Our approach (simpler): Process left-to-right, last one wins
# This matches bash behavior for the final result
```

### 5.4 Builtin Detection

```c
typedef enum e_builtin
{
    NOT_BUILTIN = 0,
    BUILTIN_ECHO,
    BUILTIN_CD,
    BUILTIN_PWD,
    BUILTIN_EXPORT,
    BUILTIN_UNSET,
    BUILTIN_ENV,
    BUILTIN_EXIT
}   t_builtin;

t_builtin   get_builtin_type(char *cmd)
{
    if (!cmd)
        return (NOT_BUILTIN);
    if (ft_strcmp(cmd, "echo") == 0)
        return (BUILTIN_ECHO);
    if (ft_strcmp(cmd, "cd") == 0)
        return (BUILTIN_CD);
    /* ... etc ... */
    return (NOT_BUILTIN);
}
```

---

## 6. Heredoc Handling

### 6.1 When to Process Heredocs

```
CRITICAL: Process ALL heredocs BEFORE forking for execution!

Why?
1. Heredoc reads from stdin (same as your prompt)
2. If you fork first, child and parent fight for stdin
3. Signals during heredoc need special handling
```

### 6.2 Heredoc Flow

```
Command: cat << EOF << END
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  1. Find all HEREDOC tokens in command list                  │
└──────────────────────────────────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  2. For each heredoc (left to right):                        │
│     a. Create temp file or pipe                              │
│     b. Read lines until delimiter                            │
│     c. Write to temp file/pipe                               │
│     d. Store FD in command struct                            │
└──────────────────────────────────────────────────────────────┘
                │
                ▼
┌──────────────────────────────────────────────────────────────┐
│  3. Last heredoc FD becomes stdin for command                │
└──────────────────────────────────────────────────────────────┘
```

### 6.3 Heredoc + Signals

```c
/* During heredoc input, Ctrl+C should: */
/* 1. Stop reading heredoc */
/* 2. NOT execute the command */
/* 3. Return to prompt with exit status 130 */

char *read_heredoc_line(char *delimiter)
{
    char *line;

    line = readline("> ");

    /* Check for Ctrl+C */
    if (g_signum == SIGINT)
    {
        free(line);
        return (NULL);  /* Signal to stop heredoc */
    }

    /* Check for Ctrl+D (EOF) */
    if (line == NULL)
    {
        /* Bash warning: here-document delimited by end-of-file */
        ft_putstr_fd("minishell: warning: here-document delimited by EOF\n", 2);
        return (NULL);
    }

    return (line);
}
```

### 6.4 Heredoc Expansion Rules

```bash
# Delimiter WITHOUT quotes: Expansion happens
cat << EOF
$HOME
EOF
# Output: /home/user

# Delimiter WITH quotes: No expansion (literal)
cat << 'EOF'
$HOME
EOF
# Output: $HOME

cat << "EOF"
$HOME
EOF
# Output: $HOME (same as single quotes for delimiter)
```

---

## 7. Executor (The Core Engine)

### 7.1 Decision Tree

```
                    ┌─────────────────────┐
                    │  Execute Commands   │
                    └──────────┬──────────┘
                               │
                    ┌──────────┴──────────┐
                    │  Count commands     │
                    │  in pipeline        │
                    └──────────┬──────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
       ┌──────────┐     ┌──────────┐     ┌──────────┐
       │ 0 cmds   │     │ 1 cmd    │     │ 2+ cmds  │
       │ (empty)  │     │ (simple) │     │ (pipe)   │
       └────┬─────┘     └────┬─────┘     └────┬─────┘
            │                │                │
            ▼                ▼                ▼
        Return 0           Execute         Execute
                           Single          Pipeline
```

### 7.2 Single Command Execution

```
┌──────────────────────────────────────────────────────────────┐
│                   SINGLE COMMAND                             │
└──────────────────────────────────────────────────────────────┘
                          │
          ┌───────────────┴───────────────┐
          │     Is it a builtin?          │
          └───────────────┬───────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
         ▼                ▼                ▼
  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
  │ cd/export/  │  │ echo/pwd/   │  │ External    │
  │ unset/exit  │  │ env         │  │ Binary      │
  │ (State-     │  │ (No-state   │  │ (ls, cat)   │
  │  changing)  │  │  builtin)   │  │             │
  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘
         │                │                │
         ▼                ▼                ▼
  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
  │ RUN IN      │  │ Can run in  │  │ MUST fork   │
  │ PARENT      │  │ parent OR   │  │             │
  │ (no fork)   │  │ fork        │  │             │
  └─────────────┘  └─────────────┘  └─────────────┘
```

**Why run cd/export/unset/exit in parent?**

- `cd`: Must change parent's working directory
- `export`: Must modify parent's environment
- `unset`: Must modify parent's environment
- `exit`: Must exit the parent shell

**Simplification for 42:** Run ALL builtins in parent for single commands. It's easier and matches bash behavior.

### 7.3 Single Command with Redirections

```c
int execute_single_command(t_command *cmd, t_shell *shell)
{
    int stdin_backup;
    int stdout_backup;
    int status;

    /* Step 1: Backup original FDs */
    stdin_backup = dup(STDIN_FILENO);
    stdout_backup = dup(STDOUT_FILENO);

    /* Step 2: Apply redirections */
    if (apply_redirections(cmd) == -1)
    {
        restore_fds(stdin_backup, stdout_backup);
        return (1);
    }

    /* Step 3: Execute builtin/external */
    if (cmd->is_builtin)
        status = execute_builtin(cmd, shell);
    else
        status = execute_external(cmd, shell);

    /* Step 4: Restore original FDs */
    restore_fds(stdin_backup, stdout_backup);

    return (status);
}
```

### 7.4 Pipeline Execution

```
Command: ls -la | grep ".c" | wc -l

┌──────────────────────────────────────────────────────────────┐
│  PARENT PROCESS                                              │
│  ─────────────────────────────────────────────────────────── │
│  1. Count commands (3)                                       │
│  2. Create pipes: pipe1[2], pipe2[2]                         │
│  3. Fork child for each command                              │
│  4. Close ALL pipe ends in parent                            │
│  5. waitpid for all children                                 │
│  6. Get exit status from LAST child                          │
└──────────────────────────────────────────────────────────────┘
         │
         ├──────────────────┬──────────────────┐
         ▼                  ▼                  ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   CHILD 1   │     │   CHILD 2   │     │   CHILD 3   │
│   ls -la    │────▶│  grep ".c"  │────▶│   wc -l     │
│             │pipe1│             │pipe2│             │
│ stdout→pipe1│     │stdin←pipe1  │     │stdin←pipe2  │
│             │     │stdout→pipe2 │     │             │
└─────────────┘     └─────────────┘     └─────────────┘
```

### 7.5 Pipeline Code Pattern

```c
void execute_pipeline(t_command *cmds, t_shell *shell)
{
    int     pipe_fd[2];
    int     prev_fd = -1;  /* Read end of previous pipe */
    pid_t   *pids;
    int     i = 0;
    t_command *cmd = cmds;

    pids = malloc(sizeof(pid_t) * count_commands(cmds));

    while (cmd)
    {
        /* Create pipe if not last command */
        if (cmd->next)
            pipe(pipe_fd);

        pids[i] = fork();
        if (pids[i] == 0)
        {
            /* CHILD */
            /* Setup input from previous pipe */
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            /* Setup output to next pipe */
            if (cmd->next)
            {
                close(pipe_fd[0]);  /* Close read end */
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }
            /* Apply file redirections (override pipe if present) */
            apply_redirections(cmd);
            /* Execute */
            execute_command(cmd, shell);
            exit(shell->last_exit);
        }

        /* PARENT */
        if (prev_fd != -1)
            close(prev_fd);
        if (cmd->next)
        {
            close(pipe_fd[1]);  /* Close write end */
            prev_fd = pipe_fd[0];  /* Save read end for next iteration */
        }

        cmd = cmd->next;
        i++;
    }

    /* Wait for all children */
    wait_for_children(pids, i, shell);
    free(pids);
}
```

### 7.6 Command Execution (In Child)

```c
void execute_command(t_command *cmd, t_shell *shell)
{
    char *path;

    /* Check if builtin */
    if (cmd->is_builtin)
    {
        shell->last_exit = run_builtin(cmd, shell);
        exit(shell->last_exit);
    }

    /* External command */
    if (cmd->argv[0] == NULL)
        exit(0);

    /* Find executable path */
    path = find_command_path(cmd->argv[0], shell);
    if (!path)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd(": command not found\n", 2);
        exit(127);
    }

    /* Execute */
    execve(path, cmd->argv, shell->envp);

    /* If execve returns, it failed */
    perror("minishell");
    exit(126);
}
```

### 7.7 Path Resolution

```c
char *find_command_path(char *cmd, t_shell *shell)
{
    char    *path_env;
    char    **paths;
    char    *full_path;
    int     i;

    /* Case 1: Command contains slash (absolute or relative path) */
    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }

    /* Case 2: Search in PATH */
    path_env = get_env_value(shell, "PATH");
    if (!path_env)
        return (NULL);  /* No PATH = can't find command */

    paths = ft_split(path_env, ':');
    i = 0;
    while (paths[i])
    {
        full_path = join_path(paths[i], cmd);  /* "dir" + "/" + "cmd" */
        if (access(full_path, X_OK) == 0)
        {
            free_array(paths);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    free_array(paths);
    return (NULL);
}
```

---

## 8. Exit Status Reference (Bash-Aligned)

All exit codes follow the [Bash Reference Manual](https://www.gnu.org/software/bash/manual/html_node/Exit-Status.html) and common shell conventions so that `$?` and scripted behavior match bash.

### 8.1 Summary Table

| Scenario                     | Exit Code      | Bash reference / usage                          |
| ---------------------------- | -------------- | ----------------------------------------------- |
| Command success              | `0`            | Normal success                                  |
| Command general error        | `1`            | General failure; builtin "too many args" return |
| Syntax error (shell misuse)  | `2`            | Misuse of shell builtin / syntax error           |
| Permission denied (exec)     | `126`          | File found but not executable                    |
| Command not found            | `127`          | Command not in PATH                             |
| Fatal signal N              | `128 + N`      | Child killed by signal N (e.g. 130 = SIGINT)     |
| Ctrl+C (SIGINT)              | `130`          | `128 + 2`                                       |
| Ctrl+\ (SIGQUIT)             | `131`          | `128 + 3`                                       |
| `exit` with valid arg        | `arg % 256`    | 0–255; out-of-range wraps (e.g. 256 → 0)        |
| `exit` with non-numeric      | `255`          | Print "numeric argument required" to stderr, exit 255 |
| `exit` with too many args    | (no exit)      | Print error to stderr, return 1, shell continues |

### 8.2 Where We Use Each Code

- **0** – Successful builtin or external command.
- **1** – Builtin failure (e.g. `exit 1 2`), redirection failure, or generic error.
- **2** – Syntax error (`syntax_check`), or `exit` misuse when we do **not** exit (e.g. too many args); also used by some test flows for "numeric argument required" **return** (shell keeps running).
- **126** – `execve` not attempted or failed: path is directory or not executable (`executor_child.c`).
- **127** – Command not found (`executor_child.c`).
- **128 + signal** – Child terminated by signal; e.g. **130** = SIGINT, **131** = SIGQUIT (`handle_child_exit`, `executor.c`, `executor_external.c`).
- **255** – `exit <non-numeric>`: print error to stderr then **exit(255)** (`builtin_exit`).

### 8.3 exit Builtin (Bash Reference)

- **Interactive only:** In an interactive shell, bash prints `"exit\n"` (or `"logout\n"` for login shells) to **stderr** before exiting (see `builtins/exit.def`). We do the same: `ft_putendl_fd("exit", STDERR_FILENO)` when `isatty(STDIN_FILENO)`.
- **Non-numeric argument:** Bash exits with **255** after printing "numeric argument required" to stderr. We match this.
- **Too many arguments:** Bash does not exit; it prints an error and returns 1. We match this.

---

## 9. Built-in Commands (Detailed Specs)

### 9.1 echo [-n] [args...]

```bash
# Behavior:
echo hello world        # "hello world\n"
echo -n hello           # "hello" (no newline)
echo -n -n -n hello     # "hello" (multiple -n same as one)
echo -nnnnn hello       # "hello" (bash accepts this)
echo -n -a hello        # "-a hello\n" (-a not valid, stops -n parsing)
echo ""                 # "\n" (empty string = just newline)
echo                    # "\n" (no args = just newline)
echo -n                 # "" (nothing, not even newline)
```

### 9.2 cd [path]

```bash
# Behavior:
cd /tmp                 # Change to /tmp
cd                      # Change to $HOME
cd -                    # Change to $OLDPWD, print new path
cd ""                   # Error or no-op (bash: no error, stays)
cd nonexistent          # Error: "No such file or directory"

# Must update:
# - PWD environment variable
# - OLDPWD environment variable
# - shell->cwd
```

### 9.3 pwd

```bash
# Behavior:
pwd                     # Print current working directory
# No options needed
# Use getcwd() or shell->cwd
```

### 9.4 export [name[=value]...]

```bash
# Behavior:
export                  # Print all exported vars (sorted, with "declare -x")
export VAR=value        # Set and export
export VAR              # Mark existing var for export (or create empty)
export VAR=             # Set VAR to empty string
export 1VAR=x           # Error: not a valid identifier
export VAR=hello=world  # VAR = "hello=world" (only first = splits)
```

### 9.5 unset [name...]

```bash
# Behavior:
unset VAR               # Remove VAR from environment
unset VAR1 VAR2         # Remove multiple
unset NONEXISTENT       # No error (silent)
unset 1VAR              # Error: not a valid identifier
```

### 9.6 env

```bash
# Behavior:
env                     # Print all environment variables
# No options or arguments
# Only print vars that have been exported
```

### 9.7 exit [n]

Bash reference: message `"exit"` is printed to **stderr** in interactive mode only.

```bash
# Behavior (exit codes match bash):
exit                    # Exit with last command's status
exit 0                  # Exit with 0
exit 42                 # Exit with 42
exit 256                # Exit with 0 (256 % 256)
exit -1                 # Exit with 255 (two's complement)
exit abc                # Error to stderr: "numeric argument required", then exit 255
exit 1 2 3              # Error to stderr: "too many arguments", return 1, do NOT exit
```

---

## 10. Error Messages Format

```c
/* Standard error format (match bash style): */
ft_putstr_fd("minishell: ", 2);
ft_putstr_fd(context, 2);       /* e.g., "cd" or filename */
ft_putstr_fd(": ", 2);
ft_putstr_fd(error_msg, 2);     /* e.g., "No such file or directory" */
ft_putstr_fd("\n", 2);

/* Examples: */
"minishell: cd: /nonexistent: No such file or directory"
"minishell: syntax error near unexpected token `|'"
"minishell: export: `1invalid': not a valid identifier"
"minishell: ./script: Permission denied"
"minishell: nosuchcmd: command not found"
```

---

## 11. Memory Management & Cleanup

### 11.1 Per-Loop Cleanup

```c
void reset_shell(t_shell *shell)
{
    free(shell->input);
    shell->input = NULL;
    free_tokens(shell->tokens);
    shell->tokens = NULL;
    free_commands(shell->commands);
    shell->commands = NULL;
    /* Note: Do NOT free envp or cwd here */
}
```

### 11.2 Exit Cleanup

```c
void free_all(t_shell *shell)
{
    reset_shell(shell);
    free_array(shell->envp);
    free(shell->cwd);
    free(shell->user);
    rl_clear_history();
}
```

### 11.3 Defensive Free Pattern

```c
void safe_free(void **ptr)
{
    if (ptr && *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
}
```

---

## 12. Testing Checklist

### 12.1 Basic Commands

- [ ] `ls`, `cat`, `echo`, `pwd` work
- [ ] Commands with arguments work
- [ ] Absolute paths work: `/bin/ls`
- [ ] Relative paths work: `./minishell`

### 12.2 Builtins

- [ ] `echo` with and without `-n`
- [ ] `cd` with path, no args, `-`
- [ ] `pwd` prints correct directory
- [ ] `export` shows and sets variables
- [ ] `unset` removes variables
- [ ] `env` shows environment
- [ ] `exit` with and without code

### 12.3 Redirections

- [ ] `< file` reads from file
- [ ] `> file` writes to file (creates/truncates)
- [ ] `>> file` appends to file
- [ ] `<< EOF` heredoc works
- [ ] Multiple redirections work

### 12.4 Pipes

- [ ] `ls | cat` works
- [ ] `ls | cat | wc` works
- [ ] `cat | cat | cat` works
- [ ] Pipes with builtins work

### 12.5 Expansion

- [ ] `$HOME` expands correctly
- [ ] `$?` expands to exit code
- [ ] `"$VAR"` expands in double quotes
- [ ] `'$VAR'` does NOT expand
- [ ] `$UNDEFINED` becomes empty

### 12.6 Signals

- [ ] Ctrl+C shows new prompt
- [ ] Ctrl+D exits shell
- [ ] Ctrl+\ does nothing in prompt
- [ ] Ctrl+C during `cat` kills cat

### 12.7 Edge Cases

- [ ] Empty input (just Enter)
- [ ] Only spaces/tabs
- [ ] Unclosed quotes error
- [ ] Invalid pipe syntax error
- [ ] Non-existent command error

---

## 13. Implementation Order (Recommended)

```
Phase 1: Foundation
├── [x] Shell struct and initialization
├── [x] Main loop with readline
├── [x] Basic signal handling
└── [x] Builtins (echo, cd, pwd, export, unset, env, exit)

Phase 2: Lexer & Parser
├── [ ] Tokenizer (split by operators)
├── [ ] Quote handling
├── [ ] Syntax validation
└── [ ] Command table construction

Phase 3: Expander
├── [ ] Variable expansion ($VAR)
├── [ ] Exit status expansion ($?)
├── [ ] Quote removal
└── [ ] Edge case handling

Phase 4: Executor (Simple)
├── [ ] Single external command execution
├── [ ] Path resolution
├── [ ] Single builtin with redirections
└── [ ] File redirections

Phase 5: Pipes & Heredoc
├── [ ] Pipeline execution
├── [ ] Heredoc implementation
├── [ ] Multiple redirections
└── [ ] Signal handling in children

Phase 6: Polish
├── [ ] Error messages
├── [ ] Memory leak fixes
├── [ ] Edge case handling
└── [ ] Norminette compliance
```

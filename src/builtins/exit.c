/*
 * Minimal `exit` builtin: exit the process with an optional status.
 */
#include "minishell.h"
#include <stdlib.h>
#include <stdio.h>

int builtin_exit(char **args, t_shell *shell)
{
    int status = 0;

    (void) shell;
    if (args && args[1])
        status = atoi(args[1]);
    exit(status);
    return (0); /* unreachable */
}

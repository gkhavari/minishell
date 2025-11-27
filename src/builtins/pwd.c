/*
 * Minimal `pwd` builtin scaffolding.
 */
#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>

int	builtin_pwd(char **args, t_shell *shell)
{
    char *cwd;

    (void) args;
    (void) shell;
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("pwd");
        return (1);
    }
    printf("%s\n", cwd);
    free(cwd);
    return (0);
}

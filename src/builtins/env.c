/*
 * Minimal `env` builtin scaffolding.
 */
#include "minishell.h"
#include <stdio.h>

int	builtin_env(char **args, t_shell *shell)
{
    int i;

    (void) args;
    if (!shell || !shell->envp)
        return (1);
    i = 0;
    while (shell->envp[i])
    {
        printf("%s\n", shell->envp[i]);
        i++;
    }
    return (0);
}

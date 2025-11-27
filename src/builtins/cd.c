/*
 * Minimal `cd` builtin scaffolding.
 * Uses chdir and updates shell->cwd when available.
 */
#include "minishell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	builtin_cd(char **args, t_shell *shell)
{
    char *target;
    char *newcwd;

    (void) args;
    if (!args || !shell)
        return (1);
    if (!args[1] || args[1][0] == '\0')
    {
        target = get_env(shell->envp, "HOME");
        if (!target)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return (1);
        }
    }
    else
        target = args[1];
    if (chdir(target) != 0)
    {
        perror("cd");
        return (1);
    }
    newcwd = getcwd(NULL, 0);
    if (newcwd)
    {
        free(shell->cwd);
        shell->cwd = newcwd;
    }
    return (0);
}

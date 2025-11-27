/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:07 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:08 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `cd` builtin scaffolding.
 * Uses chdir and updates shell->cwd when available.
 */
#include "minishell.h"

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

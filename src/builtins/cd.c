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
    char *target_path;
    char *new_working_dir;

    (void) args;
    if (!args || !shell)
        return (1);
    if (!args[1] || args[1][0] == '\0')
    {
        target_path = get_env_value(shell->envp, "HOME");
        if (!target_path)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return (1);
        }
    }
    else
        target_path = args[1];
    if (chdir(target_path) != 0)
    {
        perror("cd");
        return (1);
    }
    new_working_dir = getcwd(NULL, 0);
    if (new_working_dir)
    {
        free(shell->cwd);
        shell->cwd = new_working_dir;
    }
    return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:16 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:17 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `env` builtin scaffolding.
 */
#include "minishell.h"

int	builtin_env(char **args, t_shell *shell)
{
    int env_index;

    (void) args;
    if (!shell || !shell->envp)
        return (1);
    env_index = 0;
    while (shell->envp[env_index])
    {
        printf("%s\n", shell->envp[env_index]);
        env_index++;
    }
    return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:33 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `unset` builtin: remove variables from shell->envp.
 */
#include "minishell.h"
#include <string.h>


static int find_env_key_index(char **envp, const char *key)
{
    int index = 0;
    size_t key_len = strlen(key);
    while (envp && envp[index])
    {
        if (strncmp(envp[index], key, key_len) == 0 && envp[index][key_len] == '=')
            return index;
        index++;
    }
    return -1;
}

int builtin_unset(char **args, t_shell *shell)
{
    if (!args || !shell)
        return (1);
    for (int arg_index = 1; args[arg_index]; arg_index++)
    {
        int target_index = find_env_key_index(shell->envp, args[arg_index]);
        if (target_index < 0)
            continue;
        /* Count elements */
        int env_count = 0;
        while (shell->envp && shell->envp[env_count]) env_count++;
        char **new_envp = malloc(sizeof(char *) * env_count);
        if (!new_envp) return (1);
        int new_index = 0;
        for (int copy_index = 0; copy_index < env_count; copy_index++)
        {
            if (copy_index == target_index) continue;
            new_envp[new_index++] = strdup(shell->envp[copy_index]);
        }
        new_envp[new_index] = NULL;
        shell->envp = new_envp;
    }
    return (0);
}

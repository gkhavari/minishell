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


static int find_key_index(char **envp, const char *key)
{
    int i = 0;
    size_t len = strlen(key);
    while (envp && envp[i])
    {
        if (strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
            return i;
        i++;
    }
    return -1;
}

int builtin_unset(char **args, t_shell *shell)
{
    if (!args || !shell)
        return (1);
    for (int i = 1; args[i]; i++)
    {
        int idx = find_key_index(shell->envp, args[i]);
        if (idx < 0)
            continue;
        /* Count elements */
        int n = 0;
        while (shell->envp && shell->envp[n]) n++;
        char **ne = malloc(sizeof(char *) * n);
        if (!ne) return (1);
        int j = 0;
        for (int k = 0; k < n; k++)
        {
            if (k == idx) continue;
            ne[j++] = strdup(shell->envp[k]);
        }
        ne[j] = NULL;
        shell->envp = ne;
    }
    return (0);
}

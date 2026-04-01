/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:40 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/28 01:46:42 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Valid export variable name (letter/underscore + alnum/_). */
int	is_valid_export_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/** Index of key= in envp or -1. */
int	find_export_key_index(t_shell *shell, char *key, int key_len)
{
	int		i;
	int		env_key_len;

	i = 0;
	while (shell->envp[i])
	{
		env_key_len = 0;
		while (shell->envp[i][env_key_len]
			&& shell->envp[i][env_key_len] != '=')
			env_key_len++;
		if (env_key_len == key_len
			&& ft_strncmp(shell->envp[i], key, key_len) == 0)
			return (i);
		i++;
	}
	return (-1);
}

/** Append entry to envp (realloc list); FAILURE on malloc error. */
int	append_export_env(t_shell *shell, char *entry)
{
	char	**new_envp;
	int		count;
	int		i;

	count = 0;
	while (shell->envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (FAILURE);
	i = -1;
	while (++i < count)
		new_envp[i] = shell->envp[i];
	new_envp[count] = ft_strdup(entry);
	if (!new_envp[count])
		return (free(new_envp), FAILURE);
	new_envp[count + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (SUCCESS);
}

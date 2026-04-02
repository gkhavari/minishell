/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:40 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** export(1) message for invalid identifiers (shared by export.c). */
void	export_invalid_identifier_err(const char *arg)
{
	ft_dprintf(STDERR_FILENO, "export: `%s': not a valid identifier\n", arg);
}

/** Valid export variable name (letter/underscore + alnum/_). */
int	is_valid_export_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (FALSE);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (FALSE);
	i = 0;
	while (name[++i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (FALSE);
	}
	return (TRUE);
}

/** Index of key= in envp or -1. */
int	find_export_key_index(t_shell *shell, char *key, int key_len)
{
	int		i;
	int		env_key_len;

	i = -1;
	while (shell->envp[++i])
	{
		env_key_len = 0;
		while (shell->envp[i][env_key_len]
			&& shell->envp[i][env_key_len] != '=')
			env_key_len++;
		if (env_key_len == key_len
			&& ft_strncmp(shell->envp[i], key, key_len) == 0)
			return (i);
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

/** Bubble sort env lines (used by export -p listing). */
void	sort_env_strings(char **sorted, int count)
{
	int		i;
	char	*tmp;

	i = -1;
	while (++i < count - 1)
	{
		if (ft_strcmp(sorted[i], sorted[i + 1]) > 0)
		{
			tmp = sorted[i];
			sorted[i] = sorted[i + 1];
			sorted[i + 1] = tmp;
			i = -1;
		}
	}
}

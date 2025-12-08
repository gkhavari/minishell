/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:53 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/30 22:07:19 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** is_valid_unset_name - Check if name is valid identifier
** @name: variable name to check
** Return: 1 if valid, 0 otherwise
*/
static int	is_valid_unset_name(char *name)
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

/*
** find_env_index - Find index of env var by key
** @shell: shell state containing envp
** @key: variable name to find
** Return: index if found, -1 otherwise
*/
static int	find_env_index(t_shell *shell, char *key)
{
	int	i;
	int	len;

	len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], key, len) == 0
			&& shell->envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

/*
** remove_env_entry - Remove entry at index, shift remaining
** @shell: shell state
** @idx: index to remove
*/
static void	remove_env_entry(t_shell *shell, int idx)
{
	int	i;

	free(shell->envp[idx]);
	i = idx;
	while (shell->envp[i + 1])
	{
		shell->envp[i] = shell->envp[i + 1];
		i++;
	}
	shell->envp[i] = NULL;
}

/*
** builtin_unset - Remove environment variables
** @args: command arguments (args[1..n] = var names)
** @shell: shell state
** Return: 0 on success, 1 if any invalid identifier
*/
int	builtin_unset(char **args, t_shell *shell)
{
	int	i;
	int	idx;
	int	ret;

	i = 1;
	ret = 0;
	while (args[i])
	{
		if (!is_valid_unset_name(args[i]))
		{
			ft_putstr_fd("minishell: unset: '", 2);
			ft_putstr_fd(args[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			ret = 1;
		}
		else
		{
			idx = find_env_index(shell, args[i]);
			if (idx >= 0)
				remove_env_entry(shell, idx);
		}
		i++;
	}
	return (ret);
}

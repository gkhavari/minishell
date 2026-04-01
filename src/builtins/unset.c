/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:53 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/28 02:39:27 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** C-style identifier for unset name. */
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

/** Index of KEY= in envp or -1. */
static int	find_env_index(t_shell *shell, char *key)
{
	int		i;
	int		key_len;
	int		env_key_len;

	key_len = ft_strlen(key);
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

/** Remove envp[idx] and compact array. */
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

static int	unset_one_arg(char *arg, t_shell *shell)
{
	int	idx;

	if (arg[0] == '-')
	{
		ft_dprintf(STDERR_FILENO,
			"minishell: unset: -%s: invalid option\n", arg + 1);
		return (2);
	}
	if (!is_valid_unset_name(arg))
		return (SUCCESS);
	idx = find_env_index(shell, arg);
	if (idx >= 0)
		remove_env_entry(shell, idx);
	return (SUCCESS);
}

/** Remove valid identifiers from envp; ignore invalid names and bad options. */
int	builtin_unset(char **args, t_shell *shell)
{
	int	i;
	int	ret;
	int	r;

	i = 1;
	ret = 0;
	while (args[i])
	{
		r = unset_one_arg(args[i], shell);
		if (r > ret)
			ret = r;
		i++;
	}
	return (ret);
}

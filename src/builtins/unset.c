/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:53 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:21:22 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Validate an identifier name for `unset`.

 BEHAVIOR:
* Ensures the name is non-empty, starts with a letter or `_` and
* contains only alphanumeric characters or underscores afterwards.

 PARAMETERS:
* char *name: Identifier to validate.

 RETURN:
* `1` if valid, `0` otherwise.
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

/**
 DESCRIPTION:
* Find the index of an environment entry matching `key`.

 BEHAVIOR:
* Iterates `shell->envp` comparing the first `len` characters and
* verifies that the following character is `=` or end-of-string.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `envp`.
* char *key: Key to search for.

 RETURN:
* Index of matching entry, or `-1` if not found.
*/
static int	find_env_index(t_shell *shell, char *key)
{
	int		i;
	int		len;
	char	end;

	len = ft_strlen(key);
	i = 0;
	while (shell->envp[i])
	{
		end = shell->envp[i][len];
		if (ft_strncmp(shell->envp[i], key, len) == 0
			&& (end == '=' || end == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

/**
 DESCRIPTION:
* Remove an environment entry from `shell->envp` at the given index.

 BEHAVIOR:
* Frees the entry at `idx` then shifts remaining entries down by one
* position and terminates the array with NULL.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `envp` will be modified.
* int idx: Index of the entry to remove.
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

/**
 DESCRIPTION:
* Unset a single environment variable argument.

 BEHAVIOR:
* Validates the argument, reports invalid options (leading `-`) and
* removes the environment entry when present.

 PARAMETERS:
* char *arg: Identifier to unset.
* t_shell *shell: Shell runtime containing `envp`.

 RETURN:
* Status code: `0` on success, `2` for invalid option.
*/
static int	unset_one_arg(char *arg, t_shell *shell)
{
	int	idx;

	if (arg[0] == '-')
	{
		ft_putstr_fd("minishell: unset: -", 2);
		ft_putstr_fd(arg + 1, 2);
		ft_putendl_fd(": invalid option", 2);
		return (2);
	}
	if (!is_valid_unset_name(arg))
		return (0);
	idx = find_env_index(shell, arg);
	if (idx >= 0)
		remove_env_entry(shell, idx);
	return (0);
}

/**
 DESCRIPTION:
* `unset` builtin: remove environment variables.

 BEHAVIOR:
* Iterates over provided arguments and attempts to unset each one,
* aggregating return codes and returning the highest error code found.

 PARAMETERS:
* char **args: Argument vector where `args[1]..` are names to unset.
* t_shell *shell: Shell runtime whose `envp` will be modified.

 RETURN:
* Aggregate status code (0 for success, higher values indicate errors).
*/
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

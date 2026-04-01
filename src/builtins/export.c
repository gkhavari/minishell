/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:45 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	replace_or_append(t_shell *shell, char *arg, char *key)
{
	int		idx;
	char	*dup;

	idx = find_export_key_index(shell, key, ft_strlen(key));
	if (idx >= 0)
	{
		dup = ft_strdup(arg);
		if (!dup)
			return (FAILURE);
		free(shell->envp[idx]);
		shell->envp[idx] = dup;
		return (SUCCESS);
	}
	return (append_export_env(shell, arg));
}

static int	export_no_value(t_shell *shell, char *arg)
{
	if (!is_valid_export_name(arg))
	{
		ft_dprintf(STDERR_FILENO,
			"export: `%s': not a valid identifier\n", arg);
		return (FAILURE);
	}
	if (find_export_key_index(shell, arg, ft_strlen(arg)) < 0)
		return (append_export_env(shell, arg));
	return (SUCCESS);
}

/**
 * VAR+=suffix: build new value, then replace/append by variable name (key_name).
 * replace_or_append() looks up env keys using key_name length only — not full entry.
 */
static int	handle_append(t_shell *shell, char *key_name, char *eq)
{
	char	*old_val;
	char	*suff;
	char	*pfx;
	char	*full;
	int		ret;

	old_val = get_env_value(shell->envp, key_name);
	if (old_val)
		suff = ft_strjoin(old_val, eq + 1);
	else
		suff = ft_strdup(eq + 1);
	if (!suff)
		return (free(key_name), FAILURE);
	pfx = ft_strjoin(key_name, "=");
	if (!pfx)
		return (free(key_name), free(suff), FAILURE);
	full = ft_strjoin(pfx, suff);
	free(pfx);
	free(suff);
	if (!full)
		return (free(key_name), FAILURE);
	ret = replace_or_append(shell, full, key_name);
	free(key_name);
	free(full);
	return (ret);
}

static int	set_env_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;
	int		append_mode;
	int		ret;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (export_no_value(shell, arg));
	append_mode = (eq > arg && *(eq - 1) == '+');
	if (append_mode)
		key = ft_substr(arg, 0, eq - arg - 1);
	else
		key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (FAILURE);
	if (!is_valid_export_name(key))
	{
		ft_dprintf(STDERR_FILENO,
			"export: `%s': not a valid identifier\n", arg);
		return (free(key), FAILURE);
	}
	if (!append_mode)
	{
		ret = replace_or_append(shell, arg, key);
		free(key);
		return (ret);
	}
	return (handle_append(shell, key, eq));
}

/** export with no args prints sorted env; else set/export each argument. */
int	builtin_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
		return (print_sorted_env(shell));
	i = 1;
	ret = 0;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			ft_dprintf(STDERR_FILENO,
				"minishell: export: %s: invalid option\n", args[i]);
			ret = 2;
		}
		else if (set_env_var(shell, args[i]))
			ret = 1;
		i++;
	}
	return (ret);
}

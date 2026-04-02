/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:45 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
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

/**
 * VAR+=suffix: build full KEY=value, replace/append by key name.
 * replace_or_append() matches env keys by key_name length only.
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

static int	export_apply_assign(t_shell *shell, char *arg, char *key,
		char *eq)
{
	int	append_mode;
	int	ret;

	append_mode = (eq > arg && *(eq - 1) == '+');
	if (!is_valid_export_name(key))
	{
		export_invalid_identifier_err(arg);
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

static int	set_env_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (!is_valid_export_name(arg))
		{
			export_invalid_identifier_err(arg);
			return (FAILURE);
		}
		if (find_export_key_index(shell, arg, ft_strlen(arg)) < 0)
			return (append_export_env(shell, arg));
		return (SUCCESS);
	}
	if (eq > arg && *(eq - 1) == '+')
		key = ft_substr(arg, 0, eq - arg - 1);
	else
		key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (FAILURE);
	return (export_apply_assign(shell, arg, key, eq));
}

/** export with no args prints sorted env; else set/export each argument. */
int	builtin_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
		return (print_sorted_env(shell));
	i = 1;
	ret = SUCCESS;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			ft_dprintf(STDERR_FILENO,
				"minishell: export: %s: invalid option\n", args[i]);
			ret = XSYN;
		}
		else if (set_env_var(shell, args[i]))
		{
			if (ret != XSYN)
				ret = FAILURE;
		}
		i++;
	}
	return (ret);
}

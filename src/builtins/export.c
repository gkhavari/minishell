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
	int	idx;

	idx = find_export_key_index(shell, key, ft_strlen(key));
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = ft_strdup(arg);
		return (shell->envp[idx] == NULL);
	}
	return (append_export_env(shell, arg));
}

static int	export_no_value(t_shell *shell, char *arg)
{
	if (!is_valid_export_name(arg))
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (1);
	}
	if (find_export_key_index(shell, arg, ft_strlen(arg)) < 0)
		return (append_export_env(shell, arg));
	return (0);
}

static int	handle_append(t_shell *shell, char *key, char *eq)
{
	char	*old_val;
	char	*tmp;
	char	*new_entry;
	int		ret;

	old_val = get_env_value(shell->envp, key);
	if (old_val)
		tmp = ft_strjoin(old_val, eq + 1);
	else
		tmp = ft_strdup(eq + 1);
	if (!tmp)
		return (free(key), 1);
	new_entry = ft_strjoin(key, "=");
	free(key);
	if (!new_entry)
		return (free(tmp), 1);
	key = ft_strjoin(new_entry, tmp);
	free(new_entry);
	free(tmp);
	if (!key)
		return (1);
	ret = replace_or_append(shell, key, key);
	return (free(key), ret);
}

static int	set_env_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;
	int		append_mode;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (export_no_value(shell, arg));
	append_mode = (eq > arg && *(eq - 1) == '+');
	if (append_mode)
		key = ft_substr(arg, 0, eq - arg - 1);
	else
		key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (1);
	if (!is_valid_export_name(key))
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (free(key), 1);
	}
	if (!append_mode)
		return (replace_or_append(shell, arg, key), free(key), 0);
	return (handle_append(shell, key, eq));
}

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
			ft_putstr_fd("minishell: export: ", 2);
			ft_putstr_fd(args[i], 2);
			ft_putendl_fd(": invalid option", 2);
			ret = 2;
		}
		else if (set_env_var(shell, args[i]))
			ret = 1;
		i++;
	}
	return (ret);
}

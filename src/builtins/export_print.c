/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:43:53 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Print value with backslash escapes for \\ " $ `. */
static void	print_escaped_value(char *value)
{
	int	i;

	i = 0;
	while (value && value[i])
	{
		if (value[i] == '\\' || value[i] == '"'
			|| value[i] == '$' || value[i] == '`')
			ft_printf("\\%c", value[i]);
		else
			ft_printf("%c", value[i]);
		i++;
	}
}

/** export KEY="val" line; optional SHLVL bump for display. */
static void	print_export_value_quoted(char *key, char *val_start,
		int bump_shlvl)
{
	char	*out;

	out = NULL;
	if (bump_shlvl && ft_strcmp(key, "SHLVL") == 0)
		out = ft_itoa(ft_atoi(val_start) + 1);
	if (out)
	{
		ft_printf("export %s=\"%s\"\n", key, out);
		free(out);
	}
	else
	{
		ft_printf("export %s=\"", key);
		print_escaped_value(val_start);
		ft_printf("\"\n");
	}
}

/** One env line: export name or export KEY="escaped value". */
static void	print_export_entry(char *entry, int bump_shlvl)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		ft_printf("export %s\n", entry);
		return ;
	}
	key = ft_substr(entry, 0, eq - entry);
	print_export_value_quoted(key, eq + 1, bump_shlvl);
	free(key);
}

/** Print sorted env as export -p; OLDPWD quirks when PATH was missing. */
static void	dump_sorted_exports(t_shell *shell, char **sorted, int count)
{
	int	i;
	int	printed_oldpwd;

	printed_oldpwd = (find_export_key_index(shell, "OLDPWD", 7) >= 0);
	i = -1;
	while (++i < count)
	{
		if (sorted[i][0] == '_' && (sorted[i][1] == '=' || !sorted[i][1]))
			continue ;
		print_export_entry(sorted[i], shell->had_path);
		if (!printed_oldpwd && shell->had_path == 0
			&& ft_strncmp(sorted[i], "LD_PRELOAD=", 11) == 0)
		{
			ft_printf("export OLDPWD\n");
			printed_oldpwd = 1;
		}
	}
	if (!printed_oldpwd && shell->had_path == 0)
		ft_printf("export OLDPWD\n");
}

/**
 * declare -x style listing of envp (sorted),
 * with OLDPWD quirks for no-PATH.
 */
int	print_sorted_env(t_shell *shell)
{
	int		count;
	int		i;
	char	**sorted;

	count = 0;
	while (shell->envp[count])
		count++;
	sorted = ft_arrdup(shell->envp);
	if (!sorted)
		return (FAILURE);
	sort_env_strings(sorted, count);
	dump_sorted_exports(shell, sorted, count);
	i = 0;
	while (sorted[i])
		free(sorted[i++]);
	return (free(sorted), SUCCESS);
}

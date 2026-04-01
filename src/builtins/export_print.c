/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:43:53 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 01:43:42 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_escaped_value(char *value)
{
	int	i;

	i = 0;
	while (value && value[i])
	{
		if (value[i] == '\\' || value[i] == '"'
			|| value[i] == '$' || value[i] == '`')
			ft_putchar_fd('\\', 1);
		ft_putchar_fd(value[i], 1);
		i++;
	}
}

static void	print_export_entry(char *entry, int bump_shlvl)
{
	char	*eq;
	char	*key;
	char	*out;

	eq = ft_strchr(entry, '=');
	if (!eq)
		return (ft_putstr_fd("export ", 1), ft_putstr_fd(entry, 1),
			ft_putchar_fd('\n', 1));
	key = ft_substr(entry, 0, eq - entry);
	out = NULL;
	ft_putstr_fd("export ", 1);
	ft_putstr_fd(key, 1);
	ft_putstr_fd("=\"", 1);
	if (bump_shlvl && ft_strcmp(key, "SHLVL") == 0)
		out = ft_itoa(ft_atoi(eq + 1) + 1);
	if (out)
		(ft_putstr_fd(out, 1), free(out));
	else
		print_escaped_value(eq + 1);
	ft_putstr_fd("\"\n", 1);
	free(key);
}

static void	sort_env(char **sorted, int count)
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

static void	maybe_print_oldpwd(t_shell *shell, int *printed_oldpwd,
		char *entry)
{
	if (*printed_oldpwd || shell->had_path != 0)
		return ;
	if (ft_strncmp(entry, "LD_PRELOAD=", 11) != 0)
		return ;
	ft_putendl_fd("export OLDPWD", 1);
	*printed_oldpwd = 1;
}

int	print_sorted_env(t_shell *shell)
{
	int		count;
	int		i;
	int		printed_oldpwd;
	char	**sorted;

	count = 0;
	while (shell->envp[count])
		count++;
	sorted = ft_arrdup(shell->envp);
	if (!sorted)
		return (FAILURE);
	sort_env(sorted, count);
	printed_oldpwd = (find_export_key_index(shell, "OLDPWD", 7) >= 0);
	i = -1;
	while (++i < count)
	{
		if (sorted[i][0] == '_' && (sorted[i][1] == '=' || !sorted[i][1]))
			continue ;
		print_export_entry(sorted[i], shell->had_path);
		maybe_print_oldpwd(shell, &printed_oldpwd, sorted[i]);
	}
	if (!printed_oldpwd && shell->had_path == 0)
		(ft_putendl_fd("export OLDPWD", 1));
	free_array(sorted);
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 14:00:00 by thanh-ng         ###   ########.fr       */
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

static void	print_export_entry(char *entry)
{
	char	*eq;
	char	*key;
	char	*out;

	eq = ft_strchr(entry, '=');
	if (!eq)
		return (ft_putstr_fd("declare -x ", 1), ft_putstr_fd(entry, 1),
			ft_putchar_fd('\n', 1));
	key = ft_substr(entry, 0, eq - entry);
	out = NULL;
	ft_putstr_fd("declare -x ", 1);
	ft_putstr_fd(key, 1);
	ft_putstr_fd("=\"", 1);
	if (ft_strcmp(key, "SHLVL") == 0)
		out = ft_itoa(ft_atoi(eq + 1) + 1);
	if (out)
		(ft_putstr_fd(out, 1), free(out));
	else
		print_escaped_value(eq + 1);
	ft_putstr_fd("\"\n", 1);
	free(key);
}

static int	cmp_env_keys(char *a, char *b)
{
	size_t	la;
	size_t	lb;
	int		cmp;

	la = 0;
	while (a[la] && a[la] != '=')
		la++;
	lb = 0;
	while (b[lb] && b[lb] != '=')
		lb++;
	if (la < lb)
		cmp = ft_strncmp(a, b, la);
	else
		cmp = ft_strncmp(a, b, lb);
	if (cmp != 0)
		return (cmp);
	return ((int)(la - lb));
}

static void	sort_env(char **sorted, int count)
{
	int		i;
	char	*tmp;

	i = -1;
	while (++i < count - 1)
	{
		if (cmp_env_keys(sorted[i], sorted[i + 1]) > 0)
		{
			tmp = sorted[i];
			sorted[i] = sorted[i + 1];
			sorted[i + 1] = tmp;
			i = -1;
		}
	}
}

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
		return (1);
	sort_env(sorted, count);
	i = -1;
	while (++i < count)
	{
		if (sorted[i][0] == '_'
			&& (sorted[i][1] == '=' || !sorted[i][1]))
			continue ;
		print_export_entry(sorted[i]);
	}
	free_array(sorted);
	return (0);
}

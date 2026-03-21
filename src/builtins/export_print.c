/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:10:30 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 18:07:04 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Print a single `export` entry in `declare -x` format.

 BEHAVIOR:
* If the entry contains no '=' prints `declare -x key`.
* If the entry contains a value prints `declare -x key="value"`.

 PARAMETERS:
* char *entry: Environment entry string, either `key` or `key=value`.
*/
static void	print_export_entry(char *entry)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(entry, '=');
	if (!eq)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(entry, 1);
		ft_putchar_fd('\n', 1);
	}
	else
	{
		key = ft_substr(entry, 0, eq - entry);
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(key, 1);
		ft_putstr_fd("=\"", 1);
		ft_putstr_fd(eq + 1, 1);
		ft_putstr_fd("\"\n", 1);
		free(key);
	}
}

/**
 DESCRIPTION:
* Compare two environment entries by their keys for sorting.

 BEHAVIOR:
* Compares the key portion (up to '=' or end) of both strings. If keys
* compare equal up to the shorter length, the shorter key is considered
* smaller.

 PARAMETERS:
* char *a: First environment entry.
* char *b: Second environment entry.

 RETURN:
* Negative, zero or positive integer like `strcmp` indicating order.
*/
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

/**
 DESCRIPTION:
* Sort an array of environment entry strings in-place.

 BEHAVIOR:
* Performs a simple bubble-like sort using `cmp_env_keys` to compare
* entries. The array is expected to have `count` valid entries.

 PARAMETERS:
* char **sorted: Array of strings to sort.
* int count: Number of entries in the array.
*/
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

/**
 DESCRIPTION:
* Print the shell environment sorted in `declare -x` format.

 BEHAVIOR:
* Duplicates `shell->envp`, sorts the copy and prints each entry
* in `declare -x` form. Entries beginning with `_` are skipped when
* they contain only `_` or `_=`.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `envp` to print.

 RETURN:
* `0` on success, non-zero on allocation failure.
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

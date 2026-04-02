/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:30:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** One row per builtin; order matches e_builtin from B_ECHO … B_EXIT.
** Registry lives in function-local static storage (no file-scope globals).
*/
static const t_builtin_reg	*builtin_registry(void)
{
	static const t_builtin_reg	tab[] = {
	{"echo", builtin_echo},
	{"cd", builtin_cd},
	{"pwd", builtin_pwd},
	{"export", builtin_export},
	{"unset", builtin_unset},
	{"env", builtin_env},
	{"exit", builtin_exit}
	};

	return (tab);
}

/** Map command name to builtin id or B_NONE. */
t_builtin	get_builtin_type(char *cmd)
{
	const t_builtin_reg	*tab;
	size_t				i;
	size_t				n;

	if (!cmd)
		return (B_NONE);
	tab = builtin_registry();
	n = (size_t)(B_COUNT - B_ECHO);
	i = 0;
	while (i < n)
	{
		if (ft_strcmp(cmd, tab[i].name) == 0)
			return ((t_builtin)(B_ECHO + i));
		i++;
	}
	return (B_NONE);
}

/** Dispatch argv[0] via registry (uniform int (*)(char **, t_shell *)). */
int	run_builtin(char **argv, t_shell *shell)
{
	t_builtin			type;
	const t_builtin_reg	*tab;

	if (!argv || !argv[0])
		return (FAILURE);
	type = get_builtin_type(argv[0]);
	if (type == B_NONE)
		return (FAILURE);
	tab = builtin_registry();
	return (tab[type - B_ECHO].run(argv, shell));
}

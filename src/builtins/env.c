/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:26 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:20:55 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* `env` builtin: print the current environment.

 BEHAVIOR:
* If extra arguments are provided, prints an error and returns `127`.
* Otherwise iterates `shell->envp` and prints each entry that contains
* an '=' character to stdout.

 PARAMETERS:
* char **args: Argument vector passed to `env`.
* t_shell *shell: Shell runtime containing the `envp` array.

 RETURN:
* `0` on success, `1` if `envp` is missing, `127` for invalid usage.
*/
int	builtin_env(char **args, t_shell *shell)
{
	int	i;

	if (args[1])
	{
		ft_putstr_fd("env: '", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd("': No such file or directory", 2);
		return (127);
	}
	if (!shell->envp)
		return (1);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_putendl_fd(shell->envp[i], 1);
		i++;
	}
	return (0);
}

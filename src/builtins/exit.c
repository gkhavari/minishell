/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 01:24:17 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Validate args[1] as exit code; 2 if non-numeric, 1 if too many args. */
static int	check_exit_value(char **args, long long *value)
{
	if (!parse_exit_value(args[1], value))
	{
		ft_dprintf(STDERR_FILENO,
			"exit: %s: numeric argument required\n", args[1]);
		return (XSYN);
	}
	if (args[2])
	{
		ft_dprintf(STDERR_FILENO, "exit: too many arguments\n");
		return (FAILURE);
	}
	return (SUCCESS);
}

/** Exit shell; optional numeric code; errors without exit if too many args. */
int	builtin_exit(char **args, t_shell *shell)
{
	long long	value;

	if (isatty(STDIN_FILENO) == 1)
		ft_dprintf(STDERR_FILENO, "exit\n");
	if (!args[1])
		clean_exit(shell, shell->last_exit);
	shell->last_exit = check_exit_value(args, &value);
	if (shell->last_exit == FAILURE)
		return (FAILURE);
	if (shell->last_exit == XSYN)
		clean_exit(shell, XSYN);
	clean_exit(shell, (unsigned char)value);
	return (SUCCESS);
}

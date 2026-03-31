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

/*
** clean_exit - Free all shell resources and exit with given code.
*/
static void	clean_exit(t_shell *shell, int code)
{
	rl_clear_history();
	free_all(shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(code);
}

/*
** check_exit_value - Validate and convert exit argument
*/
static int	check_exit_value(char **args, long long *value)
{
	if (!parse_exit_value(args[1], value))
	{
		ft_putstr_fd("exit: ", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		return (2);
	}
	if (args[2])
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return (1);
	}
	return (0);
}

/*
** builtin_exit - Exit the shell with an optional exit code.
** Non-numeric arg: prints error and exits 2 (bash uses 255; see docs).
** Too many args: prints error, does NOT exit.
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long long	value;

	if (isatty(STDIN_FILENO) == 1)
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!args[1])
		clean_exit(shell, shell->last_exit);
	shell->last_exit = check_exit_value(args, &value);
	if (shell->last_exit == 1)
		return (1);
	if (shell->last_exit == 2)
		clean_exit(shell, 2);
	clean_exit(shell, (unsigned char)value);
	return (0);
}

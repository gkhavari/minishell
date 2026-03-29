/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 20:13:05 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Cleanly shut down the shell and exit with the provided code.

 BEHAVIOR:
* Clears readline history, frees shell resources and calls `exit(code)`.

 PARAMETERS:
* t_shell *shell: Shell runtime to free.
* int code: Exit code to return to the OS.
 
 RETURN:
 * Does not return; calls `exit(code)` to terminate the process.
*/
static void	clean_exit(t_shell *shell, int code)
{
	if (shell->stdin_backup != -1)
	{
		close(shell->stdin_backup);
		shell->stdin_backup = -1;
	}
	if (shell->stdout_backup != -1)
	{
		close(shell->stdout_backup);
		shell->stdout_backup = -1;
	}
	rl_clear_history();
	free_all(shell);
	exit(code);
}

/**
 DESCRIPTION:
* `exit` builtin implementation.

 BEHAVIOR:
* If running interactively prints "exit". Handles zero, one or more
* arguments: when one numeric argument is present, exits with that code
* (converted modulo 256). If non-numeric argument is given, prints an
* error and exits with status 2. If more than one argument is provided
* prints an error and returns without exiting.

 PARAMETERS:
* char **args: Argument vector for `exit`.
* t_shell *shell: Shell runtime used for cleanup and last exit status.

 RETURN:
* Does not return on successful exit; returns `1` when too many arguments
* are provided to indicate failure without exiting.
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long long	value;

	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!args[1])
		clean_exit(shell, shell->last_exit);
	if (!parse_exit_value(args[1], &value))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		clean_exit(shell, 2);
	}
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return (1);
	}
	clean_exit(shell, exit_mod256_from_ll(value));
	return (0);
}

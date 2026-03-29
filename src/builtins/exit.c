/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:41:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Check whether a string represents a valid integer number.

 BEHAVIOR:
* Accepts an optional leading '+' or '-' followed by at least one digit.
* Returns true only if all remaining characters are digits.

 PARAMETERS:
* char *str: Input string to validate.

 RETURN:
* `1` if `str` is a numeric string, `0` otherwise.
*/
static int	is_numeric_str(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 DESCRIPTION:
* Convert a numeric string to an exit code in the range 0..255.

 BEHAVIOR:
* Parses the numeric string modulo 256 and handles negative values
* according to POSIX (e.g., -1 -> 255).

 PARAMETERS:
* char *str: Numeric string to convert. May have leading '+' or '-'.

 RETURN:
* Integer exit code in range 0..255.
*/
static int	exit_mod256(char *str)
{
	unsigned int	val;
	int				neg;
	int				i;

	val = 0;
	neg = 0;
	i = 0;
	if (str[i] == '-')
		neg = 1;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		val = (val * 10 + (unsigned int)(str[i] - '0')) % 256;
		i++;
	}
	if (neg && val != 0)
		return ((int)(256 - val));
	return ((int)val);
}

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
		dup2(shell->stdin_backup, STDIN_FILENO);
		close(shell->stdin_backup);
		shell->stdin_backup = -1;
	}
	if (shell->stdout_backup != -1)
	{
		dup2(shell->stdout_backup, STDOUT_FILENO);
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
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!args[1])
		clean_exit(shell, shell->last_exit);
	if (!is_numeric_str(args[1]))
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
	clean_exit(shell, exit_mod256(args[1]));
	return (0);
}

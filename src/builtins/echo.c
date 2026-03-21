/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:20:51 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Check whether the argument is a valid `-n` flag for `echo`.

 BEHAVIOR:
* Examines the provided string: it must begin with a '-' followed by one
* or more 'n' characters and nothing else to be treated as the `-n` flag.

 PARAMETERS:
* char *arg: Argument string to inspect.

 RETURN:
* `1` if `arg` matches the `-n` pattern, otherwise `0`.
*/
static int	check_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	while (arg[i] == 'n')
		i++;
	if (arg[i] == '\0' && i > 1)
		return (1);
	return (0);
}

/**
 DESCRIPTION:
* Print an array of strings separated by single spaces starting at index.

 BEHAVIOR:
* Iterates `args` beginning at `start`, writing each argument to stdout
* separated by a single space. No trailing space is printed after the
* final argument.

 PARAMETERS:
* char **args: Null-terminated array of argument strings.
* int start: Index in `args` where printing should begin.
*/
static void	print_args(char **args, int start)
{
	int	i;

	i = start;
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
}

/**
 DESCRIPTION:
* Implementation of the `echo` builtin.

 BEHAVIOR:
* Supports multiple `-n` flags (e.g. `-n`, `-nn`) to suppress the trailing
* newline. Prints remaining arguments separated by spaces to stdout.

 PARAMETERS:
* char **args: Argument vector where `args[0]` is "echo".
* t_shell *shell: Shell state (unused by this builtin).

 RETURN:
* Exit status `0` on success.
*/
int	builtin_echo(char **args, t_shell *shell)
{
	int	newline;
	int	i;

	(void)shell;
	newline = 1;
	i = 1;
	while (args[i] && check_n_flag(args[i]))
	{
		newline = 0;
		i++;
	}
	print_args(args, i);
	if (newline)
		ft_putchar_fd('\n', 1);
	return (0);
}

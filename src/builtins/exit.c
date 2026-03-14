/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** is_numeric_str - Check if string is a valid numeric exit argument.
** Accepts optional single leading sign followed by one or more digits.
** Returns 1 if valid (including overflow values), 0 otherwise.
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

/*
** exit_mod256 - Compute exit code modulo 256 from numeric string.
** Uses modular arithmetic at each step to avoid overflow entirely.
** Handles negative values: (-n mod 256) = (256 - (n mod 256)) % 256.
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

/*
** clean_exit - Free all shell resources and exit with given code.
*/
static void	clean_exit(t_shell *shell, int code)
{
	rl_clear_history();
	free_all(shell);
	exit(code);
}

/*
** builtin_exit - Exit the shell with an optional exit code.
** Non-numeric arg: prints error and exits 255 (matching bash behavior).
** Too many args: prints error, does NOT exit.
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

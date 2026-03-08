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
#include <limits.h>

/*
** check_overflow - Check if adding a digit would overflow long
** Returns 1 if overflow would occur, 0 otherwise.
*/
static int	check_overflow(long result, int digit, int sign)
{
	if (sign == 1)
	{
		if (result > (LONG_MAX - digit) / 10)
			return (1);
	}
	else
	{
		if (result > (-(LONG_MIN + digit)) / 10)
			return (1);
	}
	return (0);
}

/*
** ft_atol_safe - Convert string to long with overflow detection
** Sets *overflow to 1 if the value exceeds LONG_MIN/LONG_MAX.
*/
static long	ft_atol_safe(const char *str, int *overflow)
{
	long	result;
	int		sign;
	int		i;

	result = 0;
	sign = 1;
	i = 0;
	*overflow = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (check_overflow(result, str[i] - '0', sign))
		{
			*overflow = 1;
			return (0);
		}
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

/*
** is_valid_exit_arg - Check if string is a valid numeric argument
** Must be optional sign followed by one or more digits, within long range.
*/
static int	is_valid_exit_arg(char *str)
{
	int		i;
	int		overflow;

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
	ft_atol_safe(str, &overflow);
	if (overflow)
		return (0);
	return (1);
}

/*
** clean_exit - Free all shell resources and exit with given code
** Called by builtin_exit to avoid memory leaks on exit.
*/
static void	clean_exit(t_shell *shell, int code)
{
	rl_clear_history();
	free_all(shell);
	exit(code);
}

/*
** builtin_exit - Exit the shell with an optional exit code
** Prints "exit" to match bash behavior.
** Non-numeric arg: error + exit 2.  Too many args: error, don't exit.
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long	exit_code;
	int		overflow;

	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!args[1])
		clean_exit(shell, shell->last_exit);
	if (!is_valid_exit_arg(args[1]))
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
	exit_code = ft_atol_safe(args[1], &overflow);
	clean_exit(shell, (exit_code % 256 + 256) % 256);
	return (0);
}

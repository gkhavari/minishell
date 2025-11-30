/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:32 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/30 22:07:14 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>

/*
** check_overflow - Check if adding digit would overflow
** @result: current result
** @digit: digit to add
** @sign: sign of the number (1 or -1)
** Return: 1 if overflow, 0 otherwise
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
** @str: string to convert
** @overflow: pointer to set if overflow occurs
** Return: long value (0 if overflow)
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
** is_valid_exit_arg - Check if string is valid numeric and within range
** @str: string to check
** Return: 1 if valid, 0 otherwise
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
** exit_numeric_error - Print error for non-numeric argument and exit
** @arg: the invalid argument
*/
static void	exit_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd(": numeric argument required", 2);
	exit(2);
}

/*
** builtin_exit - Exit the shell with optional exit code
** @args: command arguments (args[1] = exit code or empty)
** @shell: shell state for last_exit
** Return: 1 if too many args, otherwise exits
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long	exit_code;
	int		overflow;

	ft_putendl_fd("exit", 1);
	if (!args[1])
		exit(shell->last_exit);
	if (!is_valid_exit_arg(args[1]))
		exit_numeric_error(args[1]);
	if (args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return (1);
	}
	exit_code = ft_atol_safe(args[1], &overflow);
	exit((exit_code % 256 + 256) % 256);
}

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

static int	is_space_char(char c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

/*
** parse_exit_value - Parse exit numeric arg like bash.
** Allows leading/trailing whitespace, optional sign, and rejects overflow.
*/
static int	parse_exit_value(char *str, long long *value)
{
	unsigned long long	acc;
	unsigned long long	limit;
	int					i;
	int					sign;

	if (!str)
		return (0);
	i = 0;
	while (str[i] && is_space_char(str[i]))
		i++;
	sign = 1;
	if (str[i] == '+' || str[i] == '-')
		sign = 1 - (2 * (str[i++] == '-'));
	if (!ft_isdigit(str[i]))
		return (0);
	acc = 0;
	limit = (unsigned long long)LLONG_MAX;
	if (sign < 0)
		limit++;
	while (ft_isdigit(str[i]))
	{
		if (acc > (limit - (unsigned long long)(str[i] - '0')) / 10)
			return (0);
		acc = (acc * 10) + (unsigned long long)(str[i] - '0');
		i++;
	}
	while (str[i] && is_space_char(str[i]))
		i++;
	if (str[i] != '\0')
		return (0);
	if (sign < 0 && acc == (unsigned long long)LLONG_MAX + 1ULL)
		*value = LLONG_MIN;
	else
		*value = (long long)(acc * (unsigned long long)sign);
	return (1);
}

static int	exit_mod256(long long value)
{
	return ((unsigned char)value);
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
** Non-numeric arg: prints error and exits 2 (bash uses 255; see docs).
** Too many args: prints error, does NOT exit.
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
	clean_exit(shell, exit_mod256(value));
	return (0);
}

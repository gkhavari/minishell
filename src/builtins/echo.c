/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/30 20:29:23 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** True if arg is -n, -nn, ... (only n after first dash). */
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

/** Print args[start..] separated by single spaces. */
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

/** echo with -n / -nnn flags; always returns 0. */
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
	return (SUCCESS);
}

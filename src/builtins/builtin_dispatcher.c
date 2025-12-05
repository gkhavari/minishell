/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:30:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/03 21:06:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Check if command is a builtin
** Returns: 1 if builtin, 0 otherwise
*/
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

/*
** Run builtin command and return exit status
** argv[0] is the command name
** Returns: exit status of builtin
*/
int	run_builtin(char **argv, t_shell *shell)
{
	if (!argv || !argv[0])
		return (1);
	if (ft_strcmp(argv[0], "echo") == 0)
		return (builtin_echo(argv, shell));
	if (ft_strcmp(argv[0], "cd") == 0)
		return (builtin_cd(argv, shell));
	if (ft_strcmp(argv[0], "pwd") == 0)
		return (builtin_pwd(argv, shell));
	if (ft_strcmp(argv[0], "export") == 0)
		return (builtin_export(argv, shell));
	if (ft_strcmp(argv[0], "unset") == 0)
		return (builtin_unset(argv, shell));
	if (ft_strcmp(argv[0], "env") == 0)
		return (builtin_env(argv, shell));
	if (ft_strcmp(argv[0], "exit") == 0)
		return (builtin_exit(argv, shell));
	return (1);
}

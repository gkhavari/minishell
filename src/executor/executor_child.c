/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 14:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <errno.h>

static void	check_is_dir(char *cmd_name, char *path)
{
	struct stat	sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		free(path);
		exit(126);
	}
}

static void	handle_exec_error(char *cmd_name, char *path)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_name, 2);
	if (errno == ENOENT)
	{
		ft_putstr_fd(": No such file or directory\n", 2);
		free(path);
		exit(127);
	}
	ft_putstr_fd(": Permission denied\n", 2);
	free(path);
	exit(126);
}

static void	cmd_not_found(char *cmd_name)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd_name, 2);
	ft_putstr_fd(": command not found\n", 2);
	exit(127);
}

void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_builtin)
		exit(run_builtin(cmd->argv, shell));
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		cmd_not_found(cmd->argv[0]);
	check_is_dir(cmd->argv[0], path);
	set_underscore(shell, path);
	execve(path, cmd->argv, shell->envp);
	handle_exec_error(cmd->argv[0], path);
}

void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

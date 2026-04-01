/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_exec.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:38 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 17:24:03 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_builtin_child(t_command *cmd, t_shell *shell)
{
	signal(SIGPIPE, SIG_IGN);
	clean_exit(shell, run_builtin(cmd->argv, shell));
}

static void	check_is_dir(t_shell *shell, char *cmd_name, char *path)
{
	struct stat	sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		write_err3("", cmd_name, ": Is a directory\n");
		clean_exit(shell, 126);
	}
}

static void	handle_exec_error(t_shell *shell, char *cmd_name)
{
	if (errno == ENOENT)
	{
		write_err3("", cmd_name, ": No such file or directory\n");
		clean_exit(shell, 127);
	}
	write_err3("", cmd_name, ": Permission denied\n");
	clean_exit(shell, 126);
}

static void	cmd_not_found(t_shell *shell, char *cmd_name)
{
	char	*display;

	display = format_cmd_name_for_error(cmd_name);
	if (!display)
		display = cmd_name;
	write_err3("", display, ": command not found\n");
	if (display != cmd_name)
		free(display);
	clean_exit(shell, 127);
}

void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_builtin)
		run_builtin_child(cmd, shell);
	if (!cmd->argv || !cmd->argv[0])
		clean_exit(shell, 0);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		cmd_not_found(shell, cmd->argv[0]);
	if (!shell->had_path && !get_env_value(shell->envp, "PATH")
		&& !ft_strchr(cmd->argv[0], '/') && cmd->argv[1]
		&& access(path, X_OK) != 0)
		cmd_not_found(shell, cmd->argv[0]);
	check_is_dir(shell, cmd->argv[0], path);
	execve(path, cmd->argv, shell->envp);
	handle_exec_error(shell, cmd->argv[0]);
}

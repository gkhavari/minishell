/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_input_redir(t_command *cmd)
{
	int	fd;

	if (cmd->heredoc_fd != -1 && !cmd->input_file)
	{
		dup2(cmd->heredoc_fd, STDIN_FILENO);
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	if (cmd->input_file)
	{
		fd = open(cmd->input_file, O_RDONLY);
		if (fd == -1)
			return (perror("minishell"), 1);
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	return (0);
}

static int	apply_output_redir(t_command *cmd)
{
	int	fd;

	if (cmd->output_file)
	{
		if (cmd->append)
			fd = open(cmd->output_file,
					O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(cmd->output_file,
					O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
			return (perror("minishell"), 1);
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (0);
}

int	apply_redirections(t_command *cmd)
{
	if (apply_input_redir(cmd))
		return (1);
	return (apply_output_redir(cmd));
}

void	restore_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

int	execute_builtin(t_command *cmd, t_shell *shell)
{
	return (run_builtin(cmd->argv, shell));
}

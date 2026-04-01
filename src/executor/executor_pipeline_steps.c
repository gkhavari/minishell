/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_steps.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:39:42 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 17:24:05 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	setup_child_fds(int prev_fd, int pipe_fd[3], int has_next,
		int barrier_write_fd)
{
	char	sync;

	if (barrier_write_fd != -1)
		close(barrier_write_fd);
	if (pipe_fd[2] != -1)
	{
		read(pipe_fd[2], &sync, 1);
		close(pipe_fd[2]);
	}
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (has_next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}

static pid_t	fork_pipeline_cmd(t_command *cmd, t_shell *shell, int prev_fd,
		int pipe_fd[3])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (perror("minishell"), -1);
	if (pid == 0)
	{
		set_signals_default();
		if (shell->barrier_write_fd != -1)
			close(shell->barrier_write_fd);
		setup_child_fds(prev_fd, pipe_fd, cmd->next != NULL,
			shell->barrier_write_fd);
		if (apply_redirections(cmd) != 0)
			clean_exit(shell, 1);
		execute_in_child(cmd, shell);
	}
	return (pid);
}

/** Fork one pipeline segment; set up stdin/stdout pipes and sync barrier. */
pid_t	run_pipe_step(t_command *cmd, t_shell *shell,
		int *prev_fd, int sync_fd[2])
{
	int		pipe_fd[3];
	pid_t	pid;

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	pipe_fd[2] = sync_fd[0];
	if (cmd->next && pipe(pipe_fd) == -1)
		return (-1);
	pid = fork_pipeline_cmd(cmd, shell, *prev_fd, pipe_fd);
	if (pid < 0)
	{
		if (cmd->next)
			(close(pipe_fd[0]), close(pipe_fd[1]));
		return (-1);
	}
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cmd->next)
		(close(pipe_fd[1]), *prev_fd = pipe_fd[0]);
	else
		*prev_fd = -1;
	return (pid);
}

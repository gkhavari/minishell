/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_step.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:39:42 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:20:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Child: optional sync-fd barrier read, then wire prev_fd to stdin
 * and the new pipe write-end to stdout when there is a next segment.
 */
static void	setup_pipe_child_fds(int prev_fd, int pipe_fd[3], int has_next)
{
	char	sync;

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

/**
 * `fork`: child wires fds, `apply_redirs`, `run_in_child`; parent returns pid.
 */
static pid_t	fork_pipe_child(t_list *cmd_node, t_shell *shell,
		int prev_fd, int pipe_fd[3])
{
	t_command	*cmd;
	int			has_next;
	pid_t		pid;

	cmd = cmd_node->content;
	has_next = (cmd_node->next != NULL);
	pid = fork();
	if (pid < 0)
		return (perror("minishell"), -1);
	if (pid == 0)
	{
		set_signals_default();
		setup_pipe_child_fds(prev_fd, pipe_fd, has_next);
		if (apply_redirs(cmd) != SUCCESS)
			exit_norl(shell, FAILURE);
		run_in_child(cmd, shell);
	}
	return (pid);
}

/** Parent: close write end; keep read end as next segment's stdin source. */
static void	advance_prev_pipe_fd(int *prev_fd, int has_next, int p0, int p1)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (has_next)
	{
		close(p1);
		*prev_fd = p0;
	}
	else
		*prev_fd = -1;
}

/**
 * Fork one stage of a pipeline: stdin from previous pipe (if any), stdout to
 * the next stage, optional sync_fd barrier read.
 */
pid_t	pipe_step(t_list *cmd_node, t_shell *shell,
		int *prev_fd, int sync_fd[2])
{
	int			pipe_fd[3];
	pid_t		pid;
	int			has_next;

	has_next = (cmd_node->next != NULL);
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	pipe_fd[2] = sync_fd[0];
	if (has_next && pipe(pipe_fd) == -1)
		return (-1);
	pid = fork_pipe_child(cmd_node, shell, *prev_fd, pipe_fd);
	if (pid < 0)
	{
		if (has_next)
			(close(pipe_fd[0]), close(pipe_fd[1]));
		return (-1);
	}
	advance_prev_pipe_fd(prev_fd, has_next, pipe_fd[0], pipe_fd[1]);
	return (pid);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:42 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:20:30 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * `waitpid` helper: when `pid` matches `last_pid`, store exit or signal status
 * in `*last_st` (pipeline last-segment rule).
 */
static int	upd_wait_st(pid_t pid, int status, pid_t last_pid, int *last_st)
{
	if (pid < 0)
	{
		if (errno == EINTR)
			return (0);
		if (errno == ECHILD)
			return (-1);
		return (0);
	}
	if (pid == last_pid)
	{
		if (WIFEXITED(status))
			*last_st = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			*last_st = (XSB + WTERMSIG(status));
	}
	return (1);
}

/** `waitpid(-1)` until `n` children reap; return last segment exit status. */
static int	wait_pipes(pid_t last_pid, int n)
{
	int		status;
	int		last_st;
	int		i;
	int		step;
	pid_t	pid;

	last_st = FAILURE;
	i = 0;
	while (i < n)
	{
		pid = waitpid(-1, &status, 0);
		step = upd_wait_st(pid, status, last_pid, &last_st);
		if (step < 0)
			break ;
		i += step;
	}
	return (last_st);
}

/**
 * Walk commands calling `pipe_step`; return count or stop on fork or pipe
 * error.
 */
static int	spawn_pipes(t_list *cmd_node, t_shell *shell,
		pid_t *last_pid, int sync_fd[2])
{
	int		prev_fd;
	int		i;
	pid_t	pid;

	prev_fd = -1;
	i = 0;
	*last_pid = -1;
	while (cmd_node)
	{
		pid = pipe_step(cmd_node, shell, &prev_fd, sync_fd);
		if (pid < 0)
			break ;
		*last_pid = pid;
		cmd_node = cmd_node->next;
		i++;
	}
	if (prev_fd != -1)
		close(prev_fd);
	return (i);
}

/**
 * Run a pipeline: optional all-not-found fast path (`pip_all_nf`), then
 * fork each segment, wait for children, return the last segment's exit status.
 */
int	run_pip(t_list *cmds, t_shell *shell)
{
	pid_t	last_pid;
	int		sync_fd[2];
	int		result;
	int		n;

	sync_fd[0] = -1;
	sync_fd[1] = -1;
	if (pip_all_nf(cmds, shell))
		return (XNF);
	set_signals_ignore();
	n = spawn_pipes(cmds, shell, &last_pid, sync_fd);
	result = FAILURE;
	if (n > 0)
		result = wait_pipes(last_pid, n);
	set_signals_interactive();
	return (result);
}

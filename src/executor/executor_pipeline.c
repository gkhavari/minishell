/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:42 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 00:30:26 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	run_pipe_step(t_command *cmd, t_shell *shell,
			int *prev_fd, int sync_fd[2]);

static int	consume_wait_result(pid_t pid, int status, pid_t last_pid,
		int *last_status)
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
			*last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			*last_status = EXIT_STATUS_FROM_SIGNAL(WTERMSIG(status));
	}
	return (1);
}

/** waitpid loop until n reap; status of last_pid becomes return value. */
static int	wait_children_last(pid_t last_pid, int n)
{
	int		status;
	int		last_status;
	int		i;
	int		step;
	pid_t	pid;

	last_status = FAILURE;
	i = 0;
	while (i < n)
	{
		pid = waitpid(-1, &status, 0);
		step = consume_wait_result(pid, status, last_pid, &last_status);
		if (step < 0)
			break ;
		i += step;
	}
	return (last_status);
}

/** Fork each cmd with pipes + sync_fd barrier; return child count. */
static int	run_pipeline_loop(t_command *cmd, t_shell *shell,
		pid_t *last_pid, int sync_fd[2])
{
	int		prev_fd;
	int		i;
	pid_t	pid;

	prev_fd = -1;
	i = 0;
	*last_pid = -1;
	while (cmd)
	{
		pid = run_pipe_step(cmd, shell, &prev_fd, sync_fd);
		if (pid < 0)
			break ;
		*last_pid = pid;
		cmd = cmd->next;
		i++;
	}
	if (prev_fd != -1)
		close(prev_fd);
	return (i);
}

/** Pipe chain: fork children, wait, return last command exit status. */
int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	pid_t	last_pid;
	int		sync_fd[2];
	int		result;
	int		n;

	sync_fd[0] = -1;
	sync_fd[1] = -1;
	shell->barrier_write_fd = -1;
	if (handle_all_not_found_pipeline(cmds, shell))
		return (EXIT_CMD_NOT_FOUND);
	set_signals_ignore();
	n = run_pipeline_loop(cmds, shell, &last_pid, sync_fd);
	result = FAILURE;
	if (n > 0)
		result = wait_children_last(last_pid, n);
	set_signals_interactive();
	return (result);
}

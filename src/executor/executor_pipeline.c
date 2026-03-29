/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	run_pipe_step(t_command *cmd, t_shell *shell,
			int *prev_fd, int start_fd, int barrier_write_fd);
void	release_pipeline_barrier(int write_fd, int count);

/*
** wait_children_last - Wait for n children and return last command status
** Uses waitpid(-1) and tracks the child PID of the last pipeline command.
*/
static int	wait_children_last(pid_t last_pid, int n)
{
	int		status;
	int		last_status;
	int		i;
	pid_t	pid;

	last_status = 1;
	i = 0;
	while (i < n)
	{
		pid = waitpid(-1, &status, 0);
		if (pid == last_pid)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last_status);
}

static int	has_pipeline_redirs(t_command *cmd)
{
	while (cmd)
	{
		if (cmd->redirs)
			return (1);
		cmd = cmd->next;
	}
	return (0);
}

/*
** run_pipeline_loop - Fork each command and connect them with pipes
** start_fd is a read end for a launch barrier shared by all children.
** Returns the number of children forked.
*/
static int	run_pipeline_loop(t_command *cmd, t_shell *shell,
		pid_t *last_pid, int start_fd, int barrier_write_fd)
{
	int		prev_fd;
	int		i;
	pid_t	pid;

	prev_fd = -1;
	i = 0;
	*last_pid = -1;
	while (cmd)
	{
		pid = run_pipe_step(cmd, shell, &prev_fd, start_fd, barrier_write_fd);
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

/*
** execute_pipeline - Execute a multi-command pipeline (cmd1 | cmd2 | ...)
** Allocates a PID array, forks all children connected by pipes,
** waits for all of them, then returns the last child's exit status.
** Parent ignores SIGINT/SIGQUIT while children run.
*/
int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	pid_t	last_pid;
	int		start_pipe[2];
	int		result;
	int		n;

	start_pipe[0] = -1;
	start_pipe[1] = -1;
	if (!has_pipeline_redirs(cmds) && pipe(start_pipe) == -1)
		start_pipe[1] = -1;
	shell->barrier_write_fd = start_pipe[1];
	set_signals_ignore();
	n = run_pipeline_loop(cmds, shell, &last_pid, start_pipe[0],
			start_pipe[1]);
	release_pipeline_barrier(start_pipe[1], n);
	if (start_pipe[0] != -1)
		close(start_pipe[0]);
	if (start_pipe[1] != -1)
		close(start_pipe[1]);
	shell->barrier_write_fd = -1;
	result = 1;
	if (n > 0)
		result = wait_children_last(last_pid, n);
	set_signals_interactive();
	return (result);
}

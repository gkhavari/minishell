/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:42 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 21:35:24 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

pid_t	run_pipe_step(t_command *cmd, t_shell *shell,
			int *prev_fd, int sync_fd[2]);
void	release_pipeline_barrier(int write_fd, int count);

/**
 DESCRIPTION:
* Wait for the last `size` child processes to terminate.

 BEHAVIOR:
* Calls `waitpid` in a loop to reap `size` children and updates the
* global `g_last_status` from the last collected child.

 PARAMETERS:
* int size: number of child processes to wait for.

 RETURN:
* None.
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

/**
 DESCRIPTION:
* Create and run the processes for each stage of a pipeline.

 BEHAVIOR:
* Iterates the command list, creates pipes as necessary, forks child
* processes to run each command step, and connects pipe ends. Parent
* side closes unused fds and finally waits for children.

 PARAMETERS:
* t_list *cmds: linked list of commands forming the pipeline.
* t_shell *shell: runtime shell state used for execution.

 RETURN:
* None.
*/
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
		if (shell->barrier_write_fd != -1)
			write(shell->barrier_write_fd, "x", 1);
		*last_pid = pid;
		cmd = cmd->next;
		i++;
	}
	if (prev_fd != -1)
		close(prev_fd);
	return (i);
}

/**
 DESCRIPTION:
* Prepare shared resources used by pipeline execution.

 BEHAVIOR:
* Currently a placeholder which can perform global setup before the
* pipeline forks (e.g., signal handling adjustments). No-op in the
* current simple implementation.

 PARAMETERS:
* None.

 RETURN:
* None.
*/
static void	setup_pipeline_barrier(t_shell *shell, int sync_fd[2])
{
	if (pipe(sync_fd) == -1)
	{
		sync_fd[0] = -1;
		sync_fd[1] = -1;
		shell->barrier_write_fd = -1;
		return ;
	}
	shell->barrier_write_fd = sync_fd[1];
}

static void	close_pipeline_barrier(t_shell *shell, int sync_fd[2], int n)
{
	release_pipeline_barrier(shell->barrier_write_fd, n);
	if (sync_fd[0] != -1)
		close(sync_fd[0]);
	if (sync_fd[1] != -1)
		close(sync_fd[1]);
	shell->barrier_write_fd = -1;
}

/**
 DESCRIPTION:
* Execute a pipeline consisting of multiple commands.

 BEHAVIOR:
* Prepares the pipeline, spawns processes for every command in the
* pipeline, closes unused pipe ends in the parent and waits for the
* children to finish. Updates shell exit status with the last child
* status.

 PARAMETERS:
* t_shell *shell: Shell runtime holding pipeline command list.

 RETURN:
* Exit status of the pipeline (status of the last command).
*/
int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	pid_t	last_pid;
	int		sync_fd[2];
	int		result;
	int		n;

	sync_fd[0] = -1;
	sync_fd[1] = -1;
	shell->barrier_write_fd = -1;
	setup_pipeline_barrier(shell, sync_fd);
	set_signals_ignore();
	n = run_pipeline_loop(cmds, shell, &last_pid, sync_fd);
	close_pipeline_barrier(shell, sync_fd, n);
	result = 1;
	if (n > 0)
		result = wait_children_last(last_pid, n);
	set_signals_interactive();
	return (result);
}

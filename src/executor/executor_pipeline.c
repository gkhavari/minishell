/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:24:52 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:25:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Configure pipe file descriptors for a pipeline child process.

 BEHAVIOR:
* If `prev_fd` != -1 duplicates it to stdin. If `has_next` is true
* duplicates the current pipe's write end to stdout. Closes fds when done.

 PARAMETERS:
* int prev_fd: Read end of the previous pipe or -1 for first command.
* int pipe_fd[2]: Current pipe fds produced by `pipe()`.
* int has_next: Non-zero when this command is not the last in pipeline.
*/
static void	setup_child_pipes(int prev_fd, int pipe_fd[2], int has_next)
{
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
 DESCRIPTION:
* Fork and prepare a pipeline child for execution.

 BEHAVIOR:
* In the child: reset signals to defaults, configure pipes, apply
* redirections and execute the command (does not return). Parent
* receives the child's PID or -1 on fork error.

 PARAMETERS:
* t_command *cmd: Command node to execute.
* t_shell *shell: Shell runtime passed to the child.
* int prev_fd: Read-end fd from previous stage or -1.
* int pipe_fd[2]: Current pipe fds for connecting to next stage.

 RETURN:
* Child PID in the parent, or -1 on fork failure.
*/
static pid_t	fork_pipeline_cmd(t_command *cmd, t_shell *shell, int prev_fd,
	int pipe_fd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return (perror("minishell"), -1);
	if (pid == 0)
	{
		set_signals_default();
		setup_child_pipes(prev_fd, pipe_fd, cmd->next != NULL);
		if (apply_redirections(cmd) != 0)
			exit(1);
		execute_in_child(cmd, shell);
	}
	return (pid);
}

/**
 DESCRIPTION:
* Handle one step of the pipeline: create pipes and fork a child.

 BEHAVIOR:
* Creates a pipe when the command is not the last, forks the child via
* `fork_pipeline_cmd`, closes/forwards fds in the parent and updates
* `prev_fd` to feed the next command. Returns 1 on success.

 PARAMETERS:
* t_command *cmd: Current command node.
* t_shell *shell: Shell runtime.
* int *prev_fd: Pointer to the current previous read-end fd.
* pid_t *pid: Output parameter to receive the forked child's PID.

 RETURN:
* `1` on success, `0` on failure.
*/
static int	handle_pipe_step(t_command *cmd, t_shell *shell, int *prev_fd,
	pid_t *pid)
{
	int	pipe_fd[2];

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	if (cmd->next && pipe(pipe_fd) == -1)
	{
		if (*prev_fd != -1)
			close(*prev_fd);
		return (0);
	}
	*pid = fork_pipeline_cmd(cmd, shell, *prev_fd, pipe_fd);
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cmd->next)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
	return (1);
}

static int	run_pipeline_loop(t_command *cmd, t_shell *shell, pid_t *pids)
{
	int	prev_fd;
	int	i;

	prev_fd = -1;
	i = 0;
	while (cmd)
	{
		if (!handle_pipe_step(cmd, shell, &prev_fd, &pids[i]))
			break ;
		cmd = cmd->next;
		i++;
	}
	return (i);
}

/**
 DESCRIPTION:
* Execute a multi-command pipeline connecting commands with pipes.

 BEHAVIOR:
* Allocates an array for child PIDs, forks each command connected by
* pipes, waits for all children and returns the last child's exit status.
* Temporarily adjusts signal handling so the parent ignores SIGINT/SIGQUIT
* while children execute.

 PARAMETERS:
* t_command *cmds: Head of the pipeline command list.
* t_shell *shell: Shell runtime used during execution.

 RETURN:
* Exit status of the last command in the pipeline, or non-zero on error.
*/
int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	pid_t	*pids;
	int		n;
	int		result;

	n = count_cmds(cmds);
	pids = malloc(sizeof(pid_t) * n);
	if (!pids)
		return (1);
	set_signals_ignore();
	n = run_pipeline_loop(cmds, shell, pids);
	result = wait_pipeline(pids, n);
	free(pids);
	set_signals_interactive();
	return (result);
}

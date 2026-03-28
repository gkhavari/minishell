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

/*
** setup_child_pipes - In a child process, wire up stdin/stdout to pipes
** prev_fd is the read-end of the previous pipe (or -1 for the first cmd).
** pipe_fd is the current pipe (write-end goes to stdout for non-last cmds).
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

/*
** fork_pipeline_cmd - Fork one child in the pipeline
** The child sets up its pipe FDs, applies file redirections on top,
** then executes the command (never returns).
** Returns the child PID to the parent, or -1 on fork failure.
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
			exit_child(shell, 1);
		execute_in_child(cmd, shell);
	}
	return (pid);
}

/*
** run_pipeline_loop - Fork each command and connect them with pipes
** Iterates through the command list, creating a pipe before each
** non-last command. Each child gets prev_fd as stdin and pipe write-end
** as stdout. Parent closes used FDs and passes the read-end forward.
** Returns the number of children forked.
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

static int	run_pipeline_loop(t_command *cmd, t_shell *shell,
		pid_t *last_pid)
{
	int	prev_fd;
	int	i;
	pid_t	pid;

	prev_fd = -1;
	i = 0;
	*last_pid = -1;
	while (cmd)
	{
		if (!handle_pipe_step(cmd, shell, &prev_fd, &pid))
			break ;
		*last_pid = pid;
		cmd = cmd->next;
		i++;
	}
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
	int		n;
	int		result;

	set_signals_ignore();
	n = run_pipeline_loop(cmds, shell, &last_pid);
	if (n <= 0)
		result = 1;
	else
		result = wait_children_last(last_pid, n);
	set_signals_interactive();
	return (result);
}

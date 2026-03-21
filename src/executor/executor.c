/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:25:24 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:25:26 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

/**
 DESCRIPTION:
* Wait for all pipeline children and return the pipeline exit status.

 BEHAVIOR:
* Implements the Bash convention: the pipeline exit status is the
* exit status of the last command in the pipeline. Waits for each PID
* in `pids[0..n-1]`, inspects termination status and returns the last
* command's exit code or signal-derived code (128 + signal).

 PARAMETERS:
* pid_t *pids: Array of child PIDs for the pipeline.
* int n: Number of PIDs/commands in the pipeline.

 RETURN:
* Exit code of the last pipeline command, or signal-derived code.
*/
int	wait_pipeline(pid_t *pids, int n)
{
	int	status;
	int	last;
	int	i;

	last = 0;
	i = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				last = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last);
}

/**
 DESCRIPTION:
* Count commands in a pipeline linked list.

 BEHAVIOR:
* Iterates the `t_command` linked list and returns the number of nodes.

 PARAMETERS:
* t_command *cmd: Head of the command list.

 RETURN:
* Number of commands in the pipeline.
*/
int	count_cmds(t_command *cmd)
{
	int	n;

	n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

static int	backup_fds(int *in, int *out)
{
	*in = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	if (*in == -1 || *out == -1)
	{
		if (*in != -1)
			close(*in);
		if (*out != -1)
			close(*out);
		return (1);
	}
	return (0);
}

/**
 DESCRIPTION:
* Execute a single command (no pipeline) in the correct context.

 BEHAVIOR:
* Backs up stdio file descriptors, applies redirections, and either
* runs builtins in the parent (so they can modify shell state) or
* forks/execs external commands. Restores file descriptors afterwards.

 PARAMETERS:
* t_command *cmd: Command to execute.
* t_shell *shell: Shell runtime state.

 RETURN:
* Exit/status code from the executed command.
*/
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	int	stdin_backup;
	int	stdout_backup;
	int	status;

	if (backup_fds(&stdin_backup, &stdout_backup))
		return (1);
	if (apply_redirections(cmd))
	{
		restore_fds(stdin_backup, stdout_backup);
		return (1);
	}
	if (!cmd->argv || !cmd->argv[0])
	{
		restore_fds(stdin_backup, stdout_backup);
		return (0);
	}
	if (cmd->is_builtin)
		status = execute_builtin(cmd, shell);
	else
		status = execute_external(cmd, shell);
	restore_fds(stdin_backup, stdout_backup);
	return (status);
}

/**
 DESCRIPTION:
* Entry point to execute pending commands stored in the shell.

 BEHAVIOR:
* If there is a single command executes it directly (builtins in parent).
* If multiple commands are present executes them as a pipeline.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `commands`.

 RETURN:
* Aggregate exit status of executed command(s).
*/
int	execute_commands(t_shell *shell)
{
	if (!shell->commands)
		return (0);
	if (!shell->commands->next)
		return (execute_single_command(shell->commands, shell));
	return (execute_pipeline(shell->commands, shell));
}

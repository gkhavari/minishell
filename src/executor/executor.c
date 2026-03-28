/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 17:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** wait_pipeline - Wait for all pipeline children and get last exit status
** Bash convention: pipeline exit status = exit status of the LAST command.
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

/*
** count_cmds - Count the number of commands in a pipeline
** Used by execute_pipeline to allocate the PID array.
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

static int	must_run_builtin_in_parent(t_command *cmd)
{
	t_builtin	type;

	type = get_builtin_type(cmd->argv[0]);
	if (type == BUILTIN_CD || type == BUILTIN_EXPORT
		|| type == BUILTIN_UNSET || type == BUILTIN_EXIT)
		return (1);
	return (0);
}

/*
** execute_single_command - Run a single command (no pipeline)
** Builtins run in the parent process (so cd/export/unset/exit work).
** External commands are forked.
** FDs are backed up and restored around redirections.
*/
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	int	stdin_backup;
	int	stdout_backup;
	int	status;
	int	need_restore;

	if (!cmd->argv || !cmd->argv[0])
	{
		need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
		if (need_restore && backup_fds(&stdin_backup, &stdout_backup))
			return (1);
		if (need_restore && apply_redirections(cmd))
		{
			restore_fds(stdin_backup, stdout_backup);
			return (1);
		}
		if (need_restore)
			restore_fds(stdin_backup, stdout_backup);
		return (0);
	}
	if (cmd->is_builtin)
	{
		if (!must_run_builtin_in_parent(cmd)
			&& (cmd->redirs != NULL || cmd->heredoc_fd != -1))
			return (execute_external(cmd, shell));
		need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
		if (need_restore && backup_fds(&stdin_backup, &stdout_backup))
			return (1);
		if (need_restore && apply_redirections(cmd))
		{
			restore_fds(stdin_backup, stdout_backup);
			return (1);
		}
		status = execute_builtin(cmd, shell);
		if (need_restore)
			restore_fds(stdin_backup, stdout_backup);
		return (status);
	}
	else
		status = execute_external(cmd, shell);
	return (status);
}

/*
** execute_commands - Entry point: decide single vs pipeline execution
** If there is only one command, run it directly (builtins stay in parent).
** If there are multiple commands, run the full pipeline with pipes.
*/
int	execute_commands(t_shell *shell)
{
	if (!shell->commands)
		return (0);
	if (!shell->commands->next)
		return (execute_single_command(shell->commands, shell));
	return (execute_pipeline(shell->commands, shell));
}

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

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (stdin_backup == -1 || stdout_backup == -1)
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

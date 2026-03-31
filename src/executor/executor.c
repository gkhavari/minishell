/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:54 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 01:59:49 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static int	run_empty_command(t_command *cmd, int *in, int *out)
{
	int	need_restore;

	need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
	if (need_restore && backup_fds(in, out))
		return (1);
	if (need_restore && apply_redirections(cmd))
	{
		restore_fds(*in, *out);
		return (1);
	}
	if (need_restore)
		restore_fds(*in, *out);
	return (0);
}

static int	run_builtin_command(t_command *cmd, t_shell *shell,
		int *in, int *out)
{
	int	type;
	int	need_restore;

	type = get_builtin_type(cmd->argv[0]);
	need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
	if ((type != BUILTIN_CD && type != BUILTIN_EXPORT
			&& type != BUILTIN_UNSET && type != BUILTIN_EXIT)
		&& need_restore)
		return (execute_external(cmd, shell));
	if (need_restore && backup_fds(in, out))
		return (1);
	if (need_restore && apply_redirections(cmd))
	{
		restore_fds(*in, *out);
		return (1);
	}
	if (need_restore)
		restore_fds(*in, *out);
	return (execute_builtin(cmd, shell));
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

	if (!cmd->argv || !cmd->argv[0])
		return (run_empty_command(cmd, &stdin_backup, &stdout_backup));
	if (cmd->is_builtin)
		return (run_builtin_command(cmd, shell, &stdin_backup, &stdout_backup));
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

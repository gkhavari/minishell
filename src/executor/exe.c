/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:54 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	backup_stdio_fds(int *in, int *out)
{
	*in = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	if (*in == -1 || *out == -1)
	{
		if (*in != -1)
			close(*in);
		if (*out != -1)
			close(*out);
		return (FAILURE);
	}
	return (SUCCESS);
}

static void	restore_stdio_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

static int	run_empty_command(t_command *cmd, int *in, int *out)
{
	int	need_restore;

	need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
	if (need_restore && backup_stdio_fds(in, out))
		return (FAILURE);
	if (need_restore && apply_redirs(cmd))
	{
		restore_stdio_fds(*in, *out);
		return (FAILURE);
	}
	if (need_restore)
		restore_stdio_fds(*in, *out);
	return (SUCCESS);
}

static int	run_single_builtin(t_command *cmd, t_shell *shell, int *in,
		int *out)
{
	int	type;
	int	need_restore;

	type = get_builtin_type(cmd->argv[0]);
	need_restore = (cmd->redirs != NULL || cmd->heredoc_fd != -1);
	if ((type != BUILTIN_CD && type != BUILTIN_EXPORT
			&& type != BUILTIN_UNSET && type != BUILTIN_EXIT)
		&& need_restore)
		return (run_external(cmd, shell));
	if (need_restore && backup_stdio_fds(in, out))
		return (FAILURE);
	if (need_restore && apply_redirs(cmd))
	{
		restore_stdio_fds(*in, *out);
		return (FAILURE);
	}
	if (need_restore)
		restore_stdio_fds(*in, *out);
	return (run_builtin(cmd->argv, shell));
}

/**
 * Run parsed commands from shell->commands: empty argv, builtin or external,
 * or pipeline. Last exit status; SUCCESS if nothing to run.
 */
int	run_commands(t_shell *shell)
{
	t_command	*cmd;
	int			stdin_backup;
	int			stdout_backup;
	int			status;

	if (!shell->commands)
		return (SUCCESS);
	if (!shell->commands->next)
	{
		cmd = shell->commands->content;
		if (!cmd->argv || !cmd->argv[0])
			return (run_empty_command(cmd, &stdin_backup, &stdout_backup));
		if (cmd->is_builtin)
			return (run_single_builtin(cmd, shell,
					&stdin_backup, &stdout_backup));
		status = run_external(cmd, shell);
		return (status);
	}
	return (run_pipeline(shell->commands, shell));
}

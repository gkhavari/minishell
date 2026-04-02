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

/** dup stdin and stdout into in and out for builtin redir restore. */
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

/** dup2 back from backups and close backup fds. */
static void	restore_stdio_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

/** No argv[0]: optional redirs/heredoc only, then restore stdio. */
static int	run_empty_command(t_command *cmd, int *in, int *out)
{
	int	need_restore;

	need_restore = (cmd->redirs != NULL || cmd->hd_fd != -1);
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

/** Builtin with redir: backup/apply/restore or delegate to run_external. */
static int	run_single_builtin(t_command *cmd, t_shell *shell,
		int *in, int *out)
{
	int	type;
	int	need_restore;

	type = get_builtin_type(cmd->argv[0]);
	need_restore = (cmd->redirs != NULL || cmd->hd_fd != -1);
	if (need_restore && type != B_CD && type != B_EXPORT
		&& type != B_UNSET && type != B_EXIT)
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
 * Run parsed commands from shell->cmds: empty argv, builtin or external,
 * or pipeline. Last exit status; SUCCESS if nothing to run.
 */
int	run_commands(t_shell *shell)
{
	t_command	*cmd;
	int			stdin_backup;
	int			stdout_backup;

	if (!shell->cmds)
		return (SUCCESS);
	if (!shell->cmds->next)
	{
		cmd = shell->cmds->content;
		if (!cmd->argv || !cmd->argv[0])
			return (run_empty_command(cmd, &stdin_backup, &stdout_backup));
		if (cmd->is_builtin)
			return (run_single_builtin(cmd, shell,
					&stdin_backup, &stdout_backup));
		return (run_external(cmd, shell));
	}
	return (run_pipeline(shell->cmds, shell));
}

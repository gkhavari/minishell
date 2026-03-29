/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:54 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 18:44:56 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Duplicate the current standard input and output file descriptors.

 BEHAVIOR:
* Calls `dup` for `STDIN_FILENO` and `STDOUT_FILENO` and stores the
* duplicates in the provided pointers. On failure closes any created
* duplicates and returns non-zero.

 PARAMETERS:
* int *in: out-parameter to receive the duplicated stdin fd.
* int *out: out-parameter to receive the duplicated stdout fd.

 RETURN:
* `0` on success, non-zero on error.
*/
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
* Execute a command struct that contains no argv (empty invocation).

 BEHAVIOR:
* If the command has redirections or a heredoc, temporarily applies
* redirections with fd backups and restores them afterwards. This
* keeps the parent process I/O state unchanged when the command is a
* no-op apart from redirections.

 PARAMETERS:
* t_command *cmd: Command structure to process.
* int *in: pointer to store stdin backup (used on restore).
* int *out: pointer to store stdout backup (used on restore).

 RETURN:
* `0` on success, non-zero on failure applying redirections.
*/
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

/**
 DESCRIPTION:
* Execute a builtin command in the parent process when appropriate.

 BEHAVIOR:
* For builtins that must execute in the parent (cd/export/unset/exit)
* this function applies redirections in a safe manner (backup +
* restore) before calling the builtin implementation. For other
* builtins that require redirections but are not special-cased, it
* falls back to running the external execution path.

 PARAMETERS:
* t_command *cmd: Command to execute.
* t_shell *shell: Shell runtime used by builtin implementations.
* int *in: fd backup for stdin.
* int *out: fd backup for stdout.

 RETURN:
* Exit status returned by the executed builtin or external fallback.
*/
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

/**
 DESCRIPTION:
* Execute a single command (non-pipeline) from the command list.

 BEHAVIOR:
* If the command is empty handles its redirections and returns. If
* it's a builtin that must run in the parent it is executed directly
* with any required redirections applied and restored. Otherwise an
* external command is launched in a child process.

 PARAMETERS:
* t_command *cmd: The command to run.
* t_shell *shell: Shell runtime providing environment and state.

 RETURN:
* The exit status of the executed command (0 on success, >0 on error).
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

/**
 DESCRIPTION:
* Entry point for executing the parsed command list on the shell.

 BEHAVIOR:
* If the shell has no commands returns 0. If there is a single
* command runs it directly (preserving builtin semantics). If there
* are multiple commands executes the full pipeline orchestration.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `commands` linked list.

 RETURN:
* Exit status of the executed commands (last command for pipelines).
*/
int	execute_commands(t_shell *shell)
{
	if (!shell->commands)
		return (0);
	if (!shell->commands->next)
		return (execute_single_command(shell->commands, shell));
	return (execute_pipeline(shell->commands, shell));
}

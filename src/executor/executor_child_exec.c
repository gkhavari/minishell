/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_exec.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:38 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_builtin_child(t_command *cmd, t_shell *shell)
{
	signal(SIGPIPE, SIG_IGN);
	clean_exit(shell, run_builtin(cmd->argv, shell));
}

/**
 * Print argv[0] + suffix to stderr; exit child after fork (never returns).
 */
static void	child_abort_cmd_error(t_shell *shell, char *argv0, int code,
		const char *suffix)
{
	ft_dprintf(STDERR_FILENO, "%s%s", argv0, suffix);
	clean_exit(shell, code);
}

static void	child_exit_not_found(t_shell *shell, char *argv0)
{
	dprintf_cmd_not_found(argv0);
	clean_exit(shell, EXIT_CMD_NOT_FOUND);
}

/** After fork: builtin, execve, or errors with clean_exit (never returns). */
void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char		*path;
	struct stat	sb;

	if (cmd->is_builtin)
		run_builtin_child(cmd, shell);
	if (!cmd->argv || !cmd->argv[0])
		clean_exit(shell, SUCCESS);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		child_exit_not_found(shell, cmd->argv[0]);
	if (!shell->had_path && !get_env_value(shell->envp, "PATH")
		&& !ft_strchr(cmd->argv[0], '/') && cmd->argv[1]
		&& access(path, X_OK) != 0)
		child_exit_not_found(shell, cmd->argv[0]);
	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
		child_abort_cmd_error(shell, cmd->argv[0], EXIT_CMD_CANNOT_EXECUTE,
			": Is a directory\n");
	execve(path, cmd->argv, shell->envp);
	if (errno == ENOENT)
		child_abort_cmd_error(shell, cmd->argv[0], EXIT_CMD_NOT_FOUND,
			": No such file or directory\n");
	child_abort_cmd_error(shell, cmd->argv[0], EXIT_CMD_CANNOT_EXECUTE,
		": Permission denied\n");
}

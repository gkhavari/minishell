/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_child.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:38 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Child: ignore SIGPIPE, exit with builtin return status. */
static void	run_builtin_in_child(t_command *cmd, t_shell *shell)
{
	signal(SIGPIPE, SIG_IGN);
	exit_norl(shell, run_builtin(cmd->argv, shell));
}

/** Print argv0+suffix to stderr, exit_norl(code). */
static void	child_abort_msg(t_shell *shell, char *argv0, int code,
		const char *suffix)
{
	ft_dprintf(STDERR_FILENO, "%s%s", argv0, suffix);
	exit_norl(shell, code);
}

/** not found message then exit_norl(XNF). */
static void	child_exit_not_found(t_shell *shell, char *argv0)
{
	put_cmd_not_found(argv0);
	exit_norl(shell, XNF);
}

/**
 * Child after fork: run a builtin in the child, execve an external, or print
 * an error and exit_norl. Does not return to the caller.
 */
void	run_in_child(t_command *cmd, t_shell *shell)
{
	char			*path;
	struct stat		sb;

	if (cmd->is_builtin)
		run_builtin_in_child(cmd, shell);
	if (!cmd->argv || !cmd->argv[0])
		exit_norl(shell, SUCCESS);
	path = resolve_cmd_path(cmd->argv[0], shell);
	if (!path)
		child_exit_not_found(shell, cmd->argv[0]);
	if (!shell->had_path && !get_env_value(shell->envp, "PATH")
		&& !ft_strchr(cmd->argv[0], '/') && cmd->argv[1]
		&& access(path, X_OK) != 0)
		child_exit_not_found(shell, cmd->argv[0]);
	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
		child_abort_msg(shell, cmd->argv[0], XNX, ": Is a directory\n");
	execve(path, cmd->argv, shell->envp);
	if (errno == ENOENT)
		child_abort_msg(shell, cmd->argv[0], XNF,
			": No such file or directory\n");
	child_abort_msg(shell, cmd->argv[0], XNX, ": Permission denied\n");
}

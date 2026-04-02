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

static void	bi_child(t_command *cmd, t_shell *shell)
{
	signal(SIGPIPE, SIG_IGN);
	clean_exit(shell, run_builtin(cmd->argv, shell));
}

static void	ch_abort(t_shell *shell, char *argv0, int code,
		const char *suffix)
{
	ft_dprintf(STDERR_FILENO, "%s%s", argv0, suffix);
	clean_exit(shell, code);
}

static void	ch_nf(t_shell *shell, char *argv0)
{
	put_cmd_not_found(argv0);
	clean_exit(shell, XNF);
}

/**
 * Child after fork: run a builtin in the child, execve an external, or print
 * an error and clean_exit. Does not return to the caller.
 */
void	run_in_child(t_command *cmd, t_shell *shell)
{
	char			*path;
	struct stat		sb;

	if (cmd->is_builtin)
		bi_child(cmd, shell);
	if (!cmd->argv || !cmd->argv[0])
		clean_exit(shell, SUCCESS);
	path = resolve_cmd_path(cmd->argv[0], shell);
	if (!path)
		ch_nf(shell, cmd->argv[0]);
	if (!shell->had_path && !get_env_value(shell->envp, "PATH")
		&& !ft_strchr(cmd->argv[0], '/') && cmd->argv[1]
		&& access(path, X_OK) != 0)
		ch_nf(shell, cmd->argv[0]);
	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
		ch_abort(shell, cmd->argv[0], XNX, ": Is a directory\n");
	execve(path, cmd->argv, shell->envp);
	if (errno == ENOENT)
		ch_abort(shell, cmd->argv[0], XNF, ": No such file or directory\n");
	ch_abort(shell, cmd->argv[0], XNX, ": Permission denied\n");
}

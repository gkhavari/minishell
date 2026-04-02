/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_child.c                                          :+:      :+:    :+:   */
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
	clean_exit(shell, EXIT_CMD_NOT_FOUND);
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
		ch_abort(shell, cmd->argv[0], EXIT_CMD_CANNOT_EXECUTE,
			": Is a directory\n");
	execve(path, cmd->argv, shell->envp);
	if (errno == ENOENT)
		ch_abort(shell, cmd->argv[0], EXIT_CMD_NOT_FOUND,
			": No such file or directory\n");
	ch_abort(shell, cmd->argv[0], EXIT_CMD_CANNOT_EXECUTE,
		": Permission denied\n");
}

static int	is_nf_cmd(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0] || cmd->is_builtin)
		return (FALSE);
	if (cmd->redirs || cmd->heredoc_delim || cmd->heredoc_fd != -1)
		return (FALSE);
	if (ft_strchr(cmd->argv[0], '/'))
		return (FALSE);
	path = resolve_cmd_path(cmd->argv[0], shell);
	if (path)
		return (FALSE);
	return (TRUE);
}

/**
 * Pipeline fast path: if every stage is a "simple" PATH miss (no redirs/heredoc),
 * print all not-found lines in the parent and return TRUE so run_pipeline can
 * return EXIT_CMD_NOT_FOUND without forking.
 */
int	pipeline_all_nf(t_list *cmds, t_shell *shell)
{
	t_list		*node;
	t_command	*cmd;

	node = cmds;
	while (node)
	{
		cmd = node->content;
		if (!is_nf_cmd(cmd, shell))
			return (FALSE);
		node = node->next;
	}
	node = cmds;
	while (node)
	{
		cmd = node->content;
		put_cmd_not_found(cmd->argv[0]);
		node = node->next;
	}
	return (TRUE);
}

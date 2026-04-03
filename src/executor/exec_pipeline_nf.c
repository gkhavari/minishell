/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_nf.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * PATH miss, no redir/heredoc, no slash: safe to not fork in parent.
 */
static int	is_simple_not_found_command(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0] || cmd->is_builtin)
		return (FALSE);
	if (cmd->redirs || cmd->hd_delim || cmd->hd_fd != -1)
		return (FALSE);
	if (ft_strchr(cmd->argv[0], '/'))
		return (FALSE);
	path = resolve_cmd_path(cmd->argv[0], shell);
	if (path)
		return (FALSE);
	return (TRUE);
}

/**
 * Pipeline fast path: every stage is a simple PATH miss (no redirs/heredoc);
 * print not-found lines in parent. Returns TRUE so `run_pip` skips fork.
 */
int	pip_all_nf(t_list *cmds, t_shell *shell)
{
	t_list		*node;
	t_command	*cmd;

	node = cmds;
	while (node)
	{
		cmd = node->content;
		if (!is_simple_not_found_command(cmd, shell))
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

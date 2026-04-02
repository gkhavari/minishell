/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pip_not_found.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_simple_not_found_cmd(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0] || cmd->is_builtin)
		return (FALSE);
	if (cmd->redirs || cmd->heredoc_delim || cmd->heredoc_fd != -1)
		return (FALSE);
	if (ft_strchr(cmd->argv[0], '/'))
		return (FALSE);
	path = find_command_path(cmd->argv[0], shell);
	if (path)
		return (FALSE);
	return (TRUE);
}

/**
 * If every cmd is a simple missing PATH lookup, print errors in parent.
 * Else return 0.
 */
int	handle_all_not_found_pipeline(t_list *cmds, t_shell *shell)
{
	t_list		*node;
	t_command	*cmd;

	node = cmds;
	while (node)
	{
		cmd = node->content;
		if (!is_simple_not_found_cmd(cmd, shell))
			return (FALSE);
		node = node->next;
	}
	node = cmds;
	while (node)
	{
		cmd = node->content;
		dprintf_cmd_not_found(cmd->argv[0]);
		node = node->next;
	}
	return (TRUE);
}

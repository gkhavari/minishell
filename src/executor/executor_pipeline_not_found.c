/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_not_found.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 20:21:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 00:31:49 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_simple_not_found_cmd(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0] || cmd->is_builtin)
		return (0);
	if (cmd->redirs || cmd->heredoc_delim || cmd->heredoc_fd != -1)
		return (0);
	if (ft_strchr(cmd->argv[0], '/'))
		return (0);
	path = find_command_path(cmd->argv[0], shell);
	if (path)
		return (0);
	return (1);
}

/**
 * If every cmd is a simple missing PATH lookup, print errors in parent.
 * Else return 0.
 */
int	handle_all_not_found_pipeline(t_command *cmds, t_shell *shell)
{
	t_command	*cmd;

	cmd = cmds;
	while (cmd)
	{
		if (!is_simple_not_found_cmd(cmd, shell))
			return (0);
		cmd = cmd->next;
	}
	cmd = cmds;
	while (cmd)
	{
		dprintf_cmd_not_found(cmd->argv[0]);
		cmd = cmd->next;
	}
	return (1);
}

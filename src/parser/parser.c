/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_parse_core(t_shell *shell)
{
	shell->commands = build_command_list(shell, shell->tokens);
	free_tokens(&shell->tokens);
	shell->tokens = NULL;
	if (!shell->commands)
	{
		shell->last_exit = FAILURE;
		return ;
	}
	if (finalize_all_commands(shell, shell->commands) == OOM)
	{
		shell->last_exit = FAILURE;
		shell->oom = 1;
		free_commands(&shell->commands);
		shell->commands = NULL;
	}
}

/**
 * syntax_check → build_command_list → finalize_all_commands; frees tokens.
 */
void	parse_input(t_shell *shell)
{
	if (!shell->tokens)
	{
		shell->commands = NULL;
		return ;
	}
	if (syntax_check(shell->tokens) == ERR)
	{
		shell->last_exit = XSYN;
		free_tokens(&shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	run_parse_core(shell);
}

/** For each command with a delimiter, read_heredoc; FAILURE on error/SIGINT. */
int	process_heredocs(t_shell *shell)
{
	t_list		*node;
	t_command	*cmd;
	int			line_no;

	node = shell->commands;
	line_no = 1;
	while (node)
	{
		cmd = node->content;
		if (cmd->heredoc_delim)
		{
			if (read_heredoc(cmd, shell, &line_no))
				return (FAILURE);
		}
		node = node->next;
	}
	return (SUCCESS);
}

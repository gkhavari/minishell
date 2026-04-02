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

static void	run_parse_core(t_shell *shell);

/**
 * Parse `shell->tokens` into `shell->cmds`. Empty list leaves cmds NULL.
 * Syntax error: XSYN and free tokens. On success, tokens are freed inside
 * `run_parse_core`.
 */
void	parse_input(t_shell *shell)
{
	if (!shell->tokens)
	{
		shell->cmds = NULL;
		return ;
	}
	if (syntax_check(shell->tokens) == FAILURE)
	{
		shell->last_exit = XSYN;
		free_tokens(&shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	run_parse_core(shell);
}

/**
 * Read each heredoc body; store read end in `cmd->hd_fd`. *line_no tracks EOF
 * warnings. FAILURE on `read_heredoc` error (SIGINT, OOM, write, etc.).
 */
int	process_heredocs(t_shell *shell)
{
	t_list		*node;
	t_command	*cmd;
	int			line_no;

	node = shell->cmds;
	line_no = 1;
	while (node)
	{
		cmd = node->content;
		if (cmd->hd_delim && read_heredoc(cmd, shell, &line_no))
			return (FAILURE);
		node = node->next;
	}
	return (SUCCESS);
}

/**
 * Call `build_command_list`, then free token list; on FAILURE or oom, clear
 * cmds and set `shell` flags accordingly.
 */
static void	run_parse_core(t_shell *shell)
{
	shell->cmds = build_command_list(shell, shell->tokens);
	free_tokens(&shell->tokens);
	shell->tokens = NULL;
	if (!shell->cmds)
	{
		shell->last_exit = FAILURE;
		return ;
	}
	if (finalize_cmds(shell, shell->cmds) == OOM)
	{
		shell->last_exit = FAILURE;
		shell->oom = 1;
		free_cmds(&shell->cmds);
		shell->cmds = NULL;
	}
}

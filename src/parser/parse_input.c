/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/03 00:00:00 by thanh-ng         ###   ########.fr       */
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

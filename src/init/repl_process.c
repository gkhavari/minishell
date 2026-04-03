/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   repl_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:55:26 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/03 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Tokenize, parse, read heredocs, then `run_commands`; updates last_exit and
 * oom flags. Lexer OOM returns immediately; parse/heredoc OOM uses `shell->oom`.
 */
void	process_input(t_shell *shell)
{
	if (tokenize_input(shell) == OOM)
	{
		shell->last_exit = FAILURE;
		return ;
	}
	parse_input(shell);
	if (shell->oom)
		return ;
	if (!shell->cmds)
		return ;
	if (process_heredocs(shell))
	{
		if (g_signum == SIGINT)
			shell->last_exit = XSINT;
		else
			shell->last_exit = FAILURE;
		return ;
	}
	shell->last_exit = run_commands(shell);
}

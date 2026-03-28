/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frontend.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 01:40:00 by github-copilot    #+#    #+#             */
/*   Updated: 2026/03/28 01:40:00 by github-copilot   ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** process_frontend - Run lexer/parser/heredoc/executor for one input line
** This isolates the front-end pipeline from the REPL loop in main.c.
*/
void	process_frontend(t_shell *shell)
{
	tokenize_input(shell);
	parse_input(shell);
	if (!shell->commands)
		return ;
	if (process_heredocs(shell))
	{
		shell->last_exit = 130;
		return ;
	}
	shell->last_exit = execute_commands(shell);
}
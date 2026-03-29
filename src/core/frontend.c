/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frontend.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:44:12 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 18:44:14 by thanh-ng         ###   ########.fr       */
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

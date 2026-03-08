/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** new_command - Allocate and zero-initialize a command node
** Returns NULL on malloc failure.
*/
t_command	*new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->argv = NULL;
	cmd->input_file = NULL;
	cmd->out_redirs = NULL;
	cmd->is_builtin = 0;
	cmd->heredoc_delim = NULL;
	cmd->heredoc_fd = -1;
	cmd->heredoc_quoted = 0;
	cmd->next = NULL;
	return (cmd);
}

/*
** parse_tokens - Convert flat token list into a linked list of commands
** Splits on PIPE tokens. After a redirection operator (< > >> <<),
** the next WORD token is consumed as the filename/delimiter and skipped
** so it does not end up as a command argument.
*/
t_command	*parse_tokens(t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command();
	if (!head)
		return (NULL);
	cmd = head;
	while (token)
	{
		if (token->type == PIPE)
		{
			cmd->next = new_command();
			if (!cmd->next)
				return (free_commands(head), NULL);
			cmd = cmd->next;
		}
		else
		{
			add_token_to_command(cmd, token);
			if (is_redirection(token->type) && token->next)
				token = token->next;
		}
		token = token->next;
	}
	return (head);
}

/*
** parse_input - Top-level parse entry called from main loop
** Runs syntax check, then builds command list and finalizes argv arrays.
** Sets shell->last_exit = 2 on syntax error.
*/
void	parse_input(t_shell *shell)
{
	if (syntax_check(shell->tokens))
	{
		shell->last_exit = 2;
		return ;
	}
	shell->commands = parse_tokens(shell->tokens);
	finalize_all_commands(shell->commands);
}

/*
** is_redirection - Check if a token type is a redirection operator
** Returns 1 for < > >> <<, 0 otherwise.
*/
int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT || type == APPEND
		|| type == HEREDOC);
}

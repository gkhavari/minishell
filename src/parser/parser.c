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
/**
 DESCRIPTION:
* Allocates and initializes a new t_command structure.
* Everything is initiallized to 0/NULL.

RETURN VALUE:
* A pointer to a newly allocated and initialized t_command structure.
* NULL if memory allocation fails.
**/
static t_command	*new_command(t_shell *shell)
{
	t_command	*cmd;

	cmd = ft_calloc(1, sizeof(t_command));
	if (!cmd)
	{
		shell->last_exit = 1;
		return (NULL);
	}
	if (cmd)
		cmd->heredoc_fd = -1;
	return (cmd);
}

static int	parse_token_step(t_shell *shell, t_command **cmd,
		t_token **token, t_command *head)
{
	int	consumed;

	if ((*token)->type == PIPE)
	{
		(*cmd)->next = new_command(shell);
		if (!(*cmd)->next)
			return (free_commands(head), FAILURE);
		*cmd = (*cmd)->next;
		*token = (*token)->next;
		return (SUCCESS);
	}
	consumed = add_token_to_command(shell, *cmd, *token);
	if (consumed == FAILURE)
		return (free_commands(head), FAILURE);
	while (consumed > 0 && *token)
	{
		*token = (*token)->next;
		consumed--;
	}
	return (SUCCESS);
}

static t_command	*parse_tokens(t_shell *shell, t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command(shell);
	if (!head)
		return (NULL);
	cmd = head;
	while (token)
	{
		if (parse_token_step(shell, &cmd, &token, head) == FAILURE)
			return (NULL);
	}
	return (head);
}

/**
 DESCRIPTION:
* High-level parsing function that takes the already-tokenized shell input 
	and constructs
	the final command list.
* It first checks syntax validity, and if no errors are present, builds 
	the command list and performs post-processing.

PARAMETERS:
* shell: A pointer to the main shell structure, containing tokens and
	a location to store parsed commands.

BEHAVIOR:
* Calls syntax_check(shell->tokens)
* If a syntax error is detected, the function immediately returns without
	altering shell->commands.
* Calls parse_tokens() to turn tokens into a command list.
* Calls finalize_all_commands() to perform final adjustments 
	(e.g., building argv, resolving redirections, etc.).
**/
void	parse_input(t_shell *shell)
{
	if (syntax_check(shell->tokens) == SYNTAX_ERR)
	{
		shell->last_exit = EXIT_SYNTAX_ERROR;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	shell->commands = parse_tokens(shell, shell->tokens);
	free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (!shell->commands)
	{
		shell->last_exit = 1;
		return ;
	}
	if (finalize_all_commands(shell, shell->commands) == FAILURE)
	{
		free_commands(shell->commands);
		shell->commands = NULL;
		shell->last_exit = 1;
	}
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

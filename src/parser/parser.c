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

	cmd = msh_calloc(shell, 1, sizeof(t_command));
	return (cmd);
}

/**
 * DESCRIPTION:
 * Adds tokens to a given command until a PIPE or the end of the list is reached.
 * Uses add_token_to_command() to process tokens.
 *
 * PARAMETERS:
 * shell - Pointer to the shell state used during token processing.
 * cmd   - Pointer to the command structure being filled.
 * token - Pointer to the first token to process.
 *
 * RETURN:
 * Pointer to the next unprocessed token after consumption, 
 * or NULL if an error occurs.
 */
static t_token	*consume_command_tokens(t_shell *shell, t_command *cmd,
	t_token *token)
{
	int	consumed;

	consumed = add_token_to_command(shell, cmd, token);
	if (consumed == FAILURE)
	{
		free_commands(cmd);
		return (NULL);
	}
	while (consumed > 0 && token)
	{
		token = token->next;
		consumed--;
	}
	return (token);
}

/**
 * DESCRIPTION:
 * Handles the creation of a new command when a PIPE token is encountered.
 * Links the new command to the current command's next pointer.
 *
 * PARAMETERS:
 * shell - Pointer to the shell state used to allocate the new command.
 * cmd   - Pointer to the current command structure.
 *
 * RETURN:
 * Pointer to the newly created command.
 */
static t_command	*add_pipe_command(t_shell *shell, t_command *cmd)
{
	cmd->next = new_command(shell);
	return (cmd->next);
}

/**
 * DESCRIPTION:
 * Iterates through a linked list of tokens and constructs a linked list of 
 	t_command structures.
 * Each PIPE token starts a new command. Other tokens are added to the
 	current command.
 * Delegates token consumption and pipe handling to helper functions for clarity.
 *
 * PARAMETERS:
 * shell - Pointer to the shell state containing token information.
 * token - Pointer to the first token in the linked list to parse.
 *
 * RETURN:
 * Pointer to the head of the constructed t_command list, or NULL on failure.
 * The list will always contain at least one command.
 */
static t_command	*parse_tokens(t_shell *shell, t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command(shell);
	cmd = head;
	while (token)
	{
		if (token->type == PIPE)
		{
			cmd = add_pipe_command(shell, cmd);
			token = token->next;
		}
		else
		{
			token = consume_command_tokens(shell, cmd, token);
			if (!token)
				return (NULL);
		}
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
	finalize_all_commands(shell, shell->commands);
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

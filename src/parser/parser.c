/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 22:42:06 by gkhavari         ###   ########.fr       */
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
 DESCRIPTION:
* Parses a linked list of tokens and builds a linked list of t_command 
	structures.
* Each occurrence of a PIPE token indicates the start of a new command, 
	causing a new t_command to be allocated.
* Non-pipe tokens are processed and assigned to the current command via
	add_token_to_command().

PARAMETERS:
* shell: A pointer to all variables used (containing the tokenlist).

RETURN:
* A pointer to the head of the newly built command list.
* The list will contain at least one command, even if no pipe tokens were found.
 **/
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
			cmd->next = new_command(shell);
			cmd = cmd->next;
		}
		else
			add_token_to_command(shell, cmd, token);
		token = token->next;
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

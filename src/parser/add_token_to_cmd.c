/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:01:16 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Processes a token and updates the corresponding fields of a
	t_command structure.
* Depending on the token type, the function either:
** Adds a word to the command’s argument list
** Sets input or output redirection
** Marks output redirection as append (>>)
** Processes a heredoc (<<)
* This function assumes that redirection and heredoc tokens always have a
	valid token->next pointing to a WORD token.
* This is guaranteed by syntax_check() before parsing.

PARAMETERS:
* cmd: Pointer to the command currently being built.
* token: The token to interpret and apply to the command.

BEHAVIOR BY TOKEN TYPE:
* WORD: Adds the token’s value to the command arguments via add_word_to_cmd().
* REDIR_IN (<): Sets cmd->input_file to the filename following the token.
* REDIR_OUT (>): Sets cmd->output_file and clears the append flag.
* APPEND (>>): Sets cmd->output_file and enables the append flag.
* HEREDOC (<<) Calls process_heredoc() using the following
	token as the delimiter.
**/
void	add_token_to_command(t_command *cmd, t_token *token)
{
	if (token->type == WORD)
		add_word_to_cmd(cmd, token->value);
	else if (token->type == REDIR_IN)
		cmd->input_file = ft_strdup(token->next->value);
	else if (token->type == REDIR_OUT)
	{
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 0;
	}
	else if (token->type == APPEND)
	{
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 1;
	}
	else if (token->type == HEREDOC)
		process_heredoc(cmd, token->next->value);
}

/**
 DESCRIPTION:
 * Appends a new argument to the command’s argument list.
 * This list is a simple linked list of t_arg nodes, each storing one
 	argument string.

PARAMETERS: 
* cmd: The command receiving the new argument.
* word: The raw argument string to duplicate and store.

BEHAVIOR:
* Allocates a new t_arg node.
* Duplicates word into new->value.
* If cmd->args is empty, the new node becomes the head.
* Otherwise, it is appended to the end of the argument list.
**/
void	add_word_to_cmd(t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

	new = malloc(sizeof(t_arg));
	new->value = ft_strdup(word);
	new->next = NULL;
	if (!cmd->args)
		cmd->args = new;
	else
	{
		tmp = cmd->args;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

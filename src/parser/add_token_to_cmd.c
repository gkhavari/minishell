/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** handle_heredoc_token - Store heredoc delimiter from the next token
** Frees any previous delimiter (handles multiple heredocs per command).
** The actual heredoc reading happens later in process_heredocs().
*/
static void	handle_heredoc_token(t_command *cmd, t_token *token)
{
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = ft_strdup(token->next->value);
	cmd->heredoc_quoted = token->next->quoted;
}

/*
** add_token_to_command - Dispatch a token into the command structure
** WORD tokens become command arguments.
** Redirection tokens (< > >> <<) set the appropriate file/delimiter.
** Note: parse_tokens() already skips the filename WORD after redirections.
*/
void	add_token_to_command(t_command *cmd, t_token *token)
{
	if (token->type == WORD)
		add_word_to_cmd(cmd, token->value);
	else if (token->type == REDIR_IN)
	{
		free(cmd->input_file);
		cmd->input_file = ft_strdup(token->next->value);
	}
	else if (token->type == REDIR_OUT)
	{
		free(cmd->output_file);
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 0;
	}
	else if (token->type == APPEND)
	{
		free(cmd->output_file);
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 1;
	}
	else if (token->type == HEREDOC)
		handle_heredoc_token(cmd, token);
}

/*
** add_word_to_cmd - Append a word to the command's argument list
** Builds a linked list of t_arg nodes, which finalize_argv() converts
** to a char** array for execve.
*/
void	add_word_to_cmd(t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

	new = malloc(sizeof(t_arg));
	if (!new)
		return ;
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

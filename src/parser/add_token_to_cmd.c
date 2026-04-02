/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Next WORD is heredoc delimiter; replaces any previous delim. */
static int	handle_heredoc_token(t_command *cmd, t_list *tok_node)
{
	char		*new_delim;
	t_token		*next_tok;

	if (!tok_node->next)
		return (FAILURE);
	next_tok = tok_node->next->content;
	if (!next_tok->value)
		return (FAILURE);
	new_delim = ft_strdup(next_tok->value);
	if (!new_delim)
		return (FAILURE);
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = new_delim;
	cmd->heredoc_quoted = next_tok->quoted;
	return (SUCCESS);
}

/**
 * Appends a new argument to the command's argument list.
 */
static int	add_word_to_cmd(t_shell *shell, t_command *cmd, char *word)
{
	t_arg	*new;
	t_list	*node;

	(void)shell;
	new = malloc(sizeof(t_arg));
	if (!new)
		return (FAILURE);
	new->value = ft_strdup(word);
	if (!new->value)
	{
		free(new);
		return (FAILURE);
	}
	node = ft_lstnew(new);
	if (!node)
	{
		free(new->value);
		free(new);
		return (FAILURE);
	}
	ft_lstadd_back(&cmd->args, node);
	return (SUCCESS);
}

/**
 * WORD → args; redir/heredoc → file or delim. Returns 1, 2, or PARSE_ERR.
 */
int	add_token_to_command(t_shell *shell, t_command *cmd, t_list *tok_node)
{
	t_token	*token;

	token = tok_node->content;
	if (token->type == WORD)
	{
		if (token->value
			&& ft_strcmp(token->value, MSH_EMPTY_EXPAND_TOKEN) == 0)
			return (1);
		if (add_word_to_cmd(shell, cmd, token->value) == FAILURE)
			return (PARSE_ERR);
		return (1);
	}
	if (token->type == HEREDOC)
	{
		if (handle_heredoc_token(cmd, tok_node) == FAILURE)
			return (PARSE_ERR);
		return (2);
	}
	return (parse_redir_token_pair(cmd, tok_node));
}

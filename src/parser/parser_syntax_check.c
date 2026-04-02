/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_check.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:00:39 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Token type as string for syntax_error messages. */
static const char	*get_token_str(t_tokentype type)
{
	if (type == PIPE)
		return ("|");
	if (type == REDIR_IN)
		return ("<");
	if (type == REDIR_OUT)
		return (">");
	if (type == APPEND)
		return (">>");
	if (type == HEREDOC)
		return ("<<");
	return ("newline");
}

/** Redir must be followed by WORD; else syntax_error ("newline" or token). */
static int	check_redir_syntax(t_list *node)
{
	t_token	*next_tok;

	if (!node->next)
		return (syntax_error("newline"));
	next_tok = node->next->content;
	if (next_tok->type != WORD)
		return (syntax_error(get_token_str(next_tok->type)));
	return (OK);
}

static int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == APPEND || type == HEREDOC);
}

/**
 * Validate pipes and redir+WORD; ERR via syntax_error() on failure.
 */
int	syntax_check(t_list *lst)
{
	t_list	*node;
	t_token	*token;

	if (!lst)
		return (OK);
	token = lst->content;
	if (token->type == PIPE)
		return (syntax_error("|"));
	node = lst;
	while (node)
	{
		token = node->content;
		if (token->type == PIPE
			&& (!node->next
				|| ((t_token *)node->next->content)->type == PIPE))
			return (syntax_error("|"));
		if (is_redirection(token->type))
		{
			if (check_redir_syntax(node))
				return (ERR);
		}
		node = node->next;
	}
	return (OK);
}

/** Print bash-style unexpected-token message to stderr; return ERR. */
int	syntax_error(const char *msg)
{
	ft_dprintf(STDERR_FILENO,
		"syntax error near unexpected token `%s'\n", msg);
	return (ERR);
}

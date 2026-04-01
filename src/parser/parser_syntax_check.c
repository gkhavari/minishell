/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_check.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:00:39 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
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
static int	check_redir_syntax(t_token *token)
{
	if (!token->next)
		return (syntax_error("newline"));
	if (token->next->type != WORD)
		return (syntax_error(get_token_str(token->next->type)));
	return (SYNTAX_OK);
}

static int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == APPEND || type == HEREDOC);
}

/**
 * Validate pipes and redir+WORD; SYNTAX_ERR via syntax_error() on failure.
 */
int	syntax_check(t_token *token)
{
	if (!token)
		return (SYNTAX_OK);
	if (token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE
			&& (!token->next || token->next->type == PIPE))
			return (syntax_error("|"));
		if (is_redirection(token->type))
		{
			if (check_redir_syntax(token))
				return (SYNTAX_ERR);
		}
		token = token->next;
	}
	return (SYNTAX_OK);
}

/** Print bash-style unexpected-token message to stderr; return SYNTAX_ERR. */
int	syntax_error(const char *msg)
{
	ft_dprintf(STDERR_FILENO,
		"syntax error near unexpected token `%s'\n", msg);
	return (SYNTAX_ERR);
}

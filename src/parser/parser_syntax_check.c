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

/*
** get_token_str - Return the printable string for a token type
** Used in syntax error messages to show which token was unexpected.
*/
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

/*
** check_redir_syntax - Validate that a redirection is followed by a WORD
** If the next token is missing, error is "newline".
** If the next token is an operator, error shows that operator.
*/
static int	check_redir_syntax(t_token *token)
{
	if (!token->next)
		return (syntax_error("newline"));
	if (token->next->type != WORD)
		return (syntax_error(get_token_str(token->next->type)));
	return (0);
}

/*
** syntax_check - Validate token list for common syntax errors
** Rules: pipe cannot be first/last, no consecutive pipes,
** redirections must be followed by a WORD token.
** Returns 0 if valid, 1 if syntax error was printed.
*/
int	syntax_check(t_token *token)
{
	if (token && token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE
			&& (!token->next || token->next->type == PIPE))
			return (syntax_error("|"));
		if (is_redirection(token->type))
		{
			if (check_redir_syntax(token))
				return (1);
		}
		token = token->next;
	}
	return (0);
}

/*
** syntax_error - Print a syntax error message to stderr
** Format matches bash: minishell: syntax error near unexpected token `X'
** Always returns 1 so callers can "return (syntax_error(...))".
*/
int	syntax_error(const char *msg)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("'\n", 2);
	return (1);
}

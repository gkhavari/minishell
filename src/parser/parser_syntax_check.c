/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_check.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:00:39 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 13:55:46 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Return the printable string for a token type.

 BEHAVIOR:
* Maps `t_tokentype` values to their user-visible string used in
* syntax error messages (e.g., `|`, `<`, `>`, `>>`, `<<`, or `newline`).

 PARAMETERS:
* t_tokentype type: Token type to convert.

 RETURN:
* Constant string representing the token for error messages.
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

/**
 DESCRIPTION:
* Validate that a redirection token is followed by a `WORD` token.

 BEHAVIOR:
* If the next token is missing reports a `newline` syntax error. If the
* next token is another operator reports that operator via `syntax_error`.

 PARAMETERS:
* t_token *token: Token node pointing at a redirection operator.

 RETURN:
* `0` on success, non-zero (SYNTAX_ERR) on syntax error.
*/
static int	check_redir_syntax(t_token *token)
{
	if (!token->next)
		return (syntax_error("newline"));
	if (token->next->type != WORD)
		return (syntax_error(get_token_str(token->next->type)));
	return (0);
}

/**
 DESCRIPTION:
* Determine whether a token type represents a redirection operator.

 PARAMETERS:
* t_tokentype type: Token type to test.

 RETURN:
* `1` if `type` is a redirection operator, otherwise `0`.
*/
static int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == APPEND || type == HEREDOC || type == REDIR_ERR_OUT);
}

/**
 DESCRIPTION:
* Check the token list for syntax errors.

 BEHAVIOR:
* Validates pipe placement (no leading/trailing/consecutive pipes) and
* ensures redirection operators are followed by a `WORD` token. On error
* calls `syntax_error` and returns `SYNTAX_ERR`.

 PARAMETERS:
* t_token *token: Head of the token linked list; may be NULL.

 RETURN:
* `SYNTAX_OK` when valid, `SYNTAX_ERR` on error.
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
				return (1);
		}
		token = token->next;
	}
	return (SYNTAX_OK);
}

/**
 DESCRIPTION:
* Print a syntax error message to stderr.

 BEHAVIOR:
* Prints: "minishell: syntax error near unexpected token `X'" where `X`
* is the provided message. Designed to be returned directly by callers.

 PARAMETERS:
* const char *msg: Message/ token string to include in the error.

 RETURN:
* `SYNTAX_ERR` (1) so callers can `return (syntax_error(...))`.
*/
int	syntax_error(const char *msg)
{
	ft_putstr_fd("syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("'\n", 2);
	return (SYNTAX_ERR);
}

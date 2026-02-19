/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_check.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:00:39 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:00:41 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == APPEND || type == HEREDOC);
}

/**
 * DESCRIPTION:
* Checks the token list for syntax errors.
* It checks for invalid pipe placement (e.g., starting or ending with a pipe,
	or two consecutive pipes) and improper redirection usage (redirection not
	followed by a WORD token).

PARAMETERS:
* token: Pointer to the first token in the lexed token list.

RETURNS:
* SYNTAX_OK if the syntax is valid.
* SYNTAX_ERR if a syntax error is found (returned from syntax_error()).

VALIDATION RULES:
* empty input is valid
* pipe rules:
	* no leading pipes
	* no trailing pipes
	* no consecutive pipes
* redirections must be followed by a WORD token

BEHAVOIR:
* When an error is detected, the function calls syntax_error() with the 
	appropriate message.
 **/
int	syntax_check(t_token *token)
{
	if (!token)
		return (SYNTAX_OK);
	if (token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE)
		{
			if (!token->next)
				return (syntax_error("|"));
			if (token->next->type == PIPE)
				return (syntax_error("|"));
		}
		if (is_redirection(token->type))
		{
			if (!token->next || token->next->type != WORD)
				return (syntax_error("newline"));
		}
		token = token->next;
	}
	return (SYNTAX_OK);
}

/**
 DESCRIPTION:
* Prints a standardized syntax error message to stderr.
* This function is called by syntax_check() upon encountering an 
	invalid token sequence.

PARAMETERS:
* msg: A short string describing the unexpected token (e.g., "|", "newline").
 **/
int	syntax_error(const char *msg)
{
	ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("'\n", 2);
	return (SYNTAX_ERR);
}

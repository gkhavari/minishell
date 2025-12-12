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

/**
 * DESCRIPTION:
* Checks the token list for syntax errors.
* It checks for invalid pipe placement (e.g., starting or ending with a pipe,
	or two consecutive pipes) and improper redirection usage (redirection not
	followed by a WORD token).

PARAMETERS:
* token: Pointer to the first token in the lexed token list.

RETURNS:
* 0 if the syntax is valid.
* 1 if a syntax error is found (returned from syntax_error()).

VALIDATION RULES:
* Input cannot start with a pipe
* Pipes cannot appear consecutively, and a pipe cannot be the last token
* Redirection operators must be followed by a WORD token

BEHAVOIR:
* When an error is detected, the function calls syntax_error() with the 
	appropriate message.
 **/
int	syntax_check(t_token *token)
{
	if (token && token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE && (!token->next || token->next->type == PIPE))
			return (syntax_error("|"));
		if ((token->type == REDIR_IN || token->type == REDIR_OUT
				|| token->type == APPEND || token->type == HEREDOC)
			&& (!token->next || token->next->type != WORD))
			return (syntax_error("newline"));
		token = token->next;
	}
	return (0);
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
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("'\n", 2);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_ops.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 23:13:19 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 23:13:21 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
 * Checks whether a character is the beginning of a shell operator token (| < >)

 PARAMETERS:
 * c: the char to check

 RETURN VALUE:
 * 1 if c is an operator character
 * 0 if not
**/
int	is_op_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

/**
 DESCRIPTION:
 * Parses an operator token starting at position s.
 * This function identifies single and double character operators 
 	and produces the appropriate token.
 * Supperted operators: | << >> < >

 PARAMETERS:
 * s: Pointer to the input string at the operator position.
 * list: Pointer to the token list where the created operator token 
 	will be appended.

 RETURN VALUE:
 * 1 or 2: number of characters that form the operator.
 * 0: no operator found
 **/
size_t	read_operator(const char *s, t_token **list)
{
	if (s[0] == '|')
		return (add_token(list, new_token(PIPE, "|")), 1);
	if (s[0] == '<' && s[1] == '<')
		return (add_token(list, new_token(HEREDOC, "<<")), 2);
	if (s[0] == '>' && s[1] == '>')
		return (add_token(list, new_token(APPEND, ">>")), 2);
	if (s[0] == '<')
		return (add_token(list, new_token(REDIR_IN, "<")), 1);
	if (s[0] == '>')
		return (add_token(list, new_token(REDIR_OUT, ">")), 1);
	return (0);
}

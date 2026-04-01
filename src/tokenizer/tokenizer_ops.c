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
 * Supported operators: | << >> < >

 PARAMETERS:
 * s: Pointer to the input string at the operator position.
 * list: Pointer to the token list where the created operator token 
 	will be appended.

 RETURN VALUE:
 * 1 or 2: number of characters that form the operator.
 * 0: no operator found
 **/
static size_t	get_op_len_type(const char *s, t_tokentype *type)
{
	if (s[0] == '|')
		return (*type = PIPE, 1);
	if (s[0] == '<' && s[1] == '<')
		return (*type = HEREDOC, 2);
	if (s[0] == '>' && s[1] == '>')
		return (*type = APPEND, 2);
	if (s[0] == '<')
		return (*type = REDIR_IN, 1);
	if (s[0] == '>' && s[1] == '|')
		return (*type = REDIR_OUT, 2);
	if (s[0] == '>')
		return (*type = REDIR_OUT, 1);
	return (0);
}

static char	*get_op_value(t_tokentype type)
{
	if (type == PIPE)
		return ("|");
	if (type == HEREDOC)
		return ("<<");
	if (type == APPEND)
		return (">>");
	if (type == REDIR_IN)
		return ("<");
	return (">");
}

size_t	read_operator(t_shell *shell, const char *s, t_token **list)
{
	t_token		*tok;
	t_tokentype	type;
	size_t		len;

	type = WORD;
	len = get_op_len_type(s, &type);
	if (len == 0)
		return (0);
	tok = new_token(shell, type, get_op_value(type));
	if (!tok)
		shell->last_exit = 1;
	else
		add_token(list, tok);
	return (len);
}

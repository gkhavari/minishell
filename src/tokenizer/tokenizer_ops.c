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
size_t	read_operator(t_shell *shell, const char *s, t_token **list)
{
	t_token		*tok;
	t_tokentype	type;
	char		*value;
	size_t		len;

	len = 0;
	value = NULL;
	type = WORD;
	if (s[0] == '|')
	{
		type = PIPE;
		value = "|";
		len = 1;
	}
	else if (s[0] == '<' && s[1] == '<')
	{
		type = HEREDOC;
		value = "<<";
		len = 2;
	}
	else if (s[0] == '>' && s[1] == '>')
	{
		type = APPEND;
		value = ">>";
		len = 2;
	}
	else if (s[0] == '<')
	{
		type = REDIR_IN;
		value = "<";
		len = 1;
	}
	else if (s[0] == '>' && s[1] == '|')
	{
		type = REDIR_OUT;
		value = ">";
		len = 2;
	}
	else if (s[0] == '>')
	{
		type = REDIR_OUT;
		value = ">";
		len = 1;
	}
	if (len == 0)
		return (0);
	tok = new_token(shell, type, value);
	if (!tok)
		shell->last_exit = 1;
	else
		add_token(list, tok);
	return (len);
}

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

/** Parse operator at s: set *type, return byte length (0 if none). */
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

/** Literal spelling for operator token value field. */
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

/**
 * Tokenize one operator at s; push token to list.
 * Returns chars consumed, 0 if none, or OOM.
 */
int	read_operator(t_shell *shell, const char *s, t_list **list)
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
		return (OOM);
	if (add_token(list, tok) == OOM)
		return (OOM);
	return ((int)len);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_string.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** ifs_mode 0: space/tab (lexer word breaks).
** ifs_mode 1: space/tab/newline (default IFS; not full ft_isspace).
*/
int	msh_is_blank(int c, int ifs_mode)
{
	if (c == ' ' || c == '\t')
		return (TRUE);
	if (ifs_mode && c == '\n')
		return (TRUE);
	return (FALSE);
}

/* Length of $NAME body from s[start]: alnum and '_' (may be 0). */
size_t	msh_env_var_body_span(const char *s, size_t start)
{
	size_t	len;

	len = 0;
	while (s[start + len] && (ft_isalnum((unsigned char)s[start + len])
			|| s[start + len] == '_'))
		len++;
	return (len);
}

/* Char after '$' that starts $? or $NAME (heredoc). */
int	msh_is_dollar_var_leader(int c)
{
	if (ft_isalpha((unsigned char)c) || c == '_' || c == '?')
		return (TRUE);
	return (FALSE);
}

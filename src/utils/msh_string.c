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
** Space or tab: readline line lexer uses only these as word breaks.
*/
int	msh_is_lexer_blank(int c)
{
	return (c == ' ' || c == '\t');
}

/*
** Default IFS blanks (space, tab, newline) for unquoted expansion split.
** Not ft_isspace: excludes \\v \\f \\r (bash default IFS).
*/
int	msh_is_ifs_blank(int c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

/** Alnum or '_' after first char of $NAME (expansion / heredoc). */
int	msh_is_env_var_body(int c)
{
	return (ft_isalnum((unsigned char)c) || c == '_');
}

/* Length of env var body run from s[start] (may be 0). */
size_t	msh_env_var_body_span(const char *s, size_t start)
{
	size_t	len;

	len = 0;
	while (s[start + len] && msh_is_env_var_body((unsigned char)s[start + len]))
		len++;
	return (len);
}

/* Char after '$' that starts $? or $NAME (heredoc). */
int	msh_is_dollar_var_leader(int c)
{
	return (ft_isalpha((unsigned char)c) || c == '_' || c == '?');
}

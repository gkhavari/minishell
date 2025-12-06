/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   continuation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 00:40:08 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/06 00:40:10 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_continuation_line(char quote_char)
{
	char	*line;
	char	*prompt;
	char	tmp[2];
	size_t	len;
	char	*with_newline;

	tmp[0] = quote_char;
	tmp[1] = '\0';
	prompt = ft_strjoin(tmp, "quote>");
	line = readline(prompt);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	with_newline = malloc(len + 2);
	if (!with_newline)
		exit(1);
	ft_memcpy(with_newline, line, len);
	with_newline[len] = '\n';
	with_newline[len + 1] = '\0';
	free(line);
	free(prompt);
	return (with_newline);
}

/**
 * Appends a continuation line to the input buffer.
 * Returns 1 on success, 0 on EOF/failure.
 */
int	append_continuation(char **s, t_state state)
{
	char	quote_char;
	char	*cont;
	size_t	old_len;
	size_t	cont_len;
	char	*new_input;

	if (state == ST_SQUOTE)
		quote_char = SINGLE_QUOTE;
	else
		quote_char = DOUBLE_QUOTE;
	cont = read_continuation_line(quote_char);
	if (!cont)
		return (0);
	old_len = ft_strlen(*s);
	cont_len = ft_strlen(cont);
	new_input = malloc(old_len + cont_len + 1);
	if (!new_input)
		exit(1);
	ft_memcpy(new_input, *s, old_len);
	ft_memcpy(new_input + old_len, cont, cont_len);
	new_input[old_len + cont_len] = '\0';
	free(cont);
	free(*s);
	*s = new_input;
	return (1);
}

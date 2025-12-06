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

static char	*read_continuation_line(char quote_char)
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

static char	get_quote_char(t_state state)
{
	if (state == ST_SQUOTE)
		return (SINGLE_QUOTE);
	return (DOUBLE_QUOTE);
}

static void	ensure_trailing_newline(char **s, size_t *old_len)
{
	char	*tmp;

	if (*old_len > 0 && (*s)[*old_len - 1] == '\n')
		return ;
	tmp = malloc(*old_len + 2);
	if (!tmp)
		exit(1);
	ft_memcpy(tmp, *s, *old_len);
	tmp[*old_len] = '\n';
	tmp[*old_len + 1] = '\0';
	free(*s);
	*s = tmp;
	(*old_len)++;
}

static void	append_to_input(char **s, char *cont, size_t old_len,
		size_t cont_len)
{
	char	*new_input;

	new_input = malloc(old_len + cont_len + 1);
	if (!new_input)
		exit(1);
	ft_memcpy(new_input, *s, old_len);
	ft_memcpy(new_input + old_len, cont, cont_len);
	new_input[old_len + cont_len] = '\0';
	free(*s);
	*s = new_input;
}

int	append_continuation(char **s, t_state state)
{
	char	quote_char;
	char	*cont;
	size_t	old_len;
	size_t	cont_len;

	quote_char = get_quote_char(state);
	cont = read_continuation_line(quote_char);
	if (!cont)
		return (0);
	old_len = ft_strlen(*s);
	cont_len = ft_strlen(cont);
	ensure_trailing_newline(s, &old_len);
	append_to_input(s, cont, old_len, cont_len);
	free(cont);
	return (1);
}

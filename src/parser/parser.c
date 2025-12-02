/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/29 20:21:02 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse_pipe(const char *s, size_t *i, t_tokentype *type)
{
	(void)s;
	*type = PIPE;
	(*i)++;
}

void	parse_heredoc_redir_in(const char *s, size_t *i, t_tokentype *type)
{
	if (s[*i + 1] == '<')
	{
		*type = HEREDOC;
		(*i)++;
	}
	else
		*type = REDIR_IN;
	(*i)++;
}

void	parse_append_redir_out(const char *s, size_t *i, t_tokentype *type)
{
	if (s[*i + 1] == '>')
	{
		*type = APPEND;
		(*i)++;
	}
	else
		*type = REDIR_OUT;
	(*i)++;
}

char	*parse_sting(const char *s, size_t *i, t_tokentype *type)
{
	size_t	j;
	char	*word;

	*type = WORD;
	j = *i + 1;
	while (s[j] && s[j] != s[*i])
		j++;
	word = NULL;
	printf("%c\n", s[*i]);
	if (s[*i] == SINGLE_QUOTE)
	{
		word = handle_single_quote(s, *i, j);
	}
/*	else if (s[j] == DOUBLE_QUOTE)
		handle_double_quote(word, s, *i, j);
	else
		continuation_prompt();*/
	*i = j + 1;
	return (word);
}

char	*parse_word(const char *s, size_t *i, t_tokentype *type)
{
	size_t	j;
	char	*word;

	*type = WORD;
	j = *i;
	while (s[*i] && s[*i] != ' ' && s[*i] != '|'
		&& s[*i] != '<' && s[*i] != '>')
		(*i)++;
	word = ft_calloc(*i - j + 1, sizeof(char));
	if (!word)
		return (NULL);
	word = ft_memcpy(word, &s[j], *i - j);
	return (word);
}

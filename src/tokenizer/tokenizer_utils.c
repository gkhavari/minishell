/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 22:41:39 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 22:41:41 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	g_word_quoted;
static int	g_heredoc_mode;

void	mark_word_quoted(void)
{
	g_word_quoted = 1;
}

void	set_heredoc_mode(int mode)
{
	g_heredoc_mode = mode;
}

int	is_heredoc_mode(void)
{
	return (g_heredoc_mode);
}

void	flush_word(char **word, t_token **token)
{
	t_token	*tok;

	if (*word)
	{
		tok = new_token(WORD, *word);
		tok->quoted = g_word_quoted;
		add_token(token, tok);
		free(*word);
		*word = NULL;
	}
	g_word_quoted = 0;
	g_heredoc_mode = 0;
}

void	append_char(char **dst, char c)
{
	size_t	len;
	char	*new;

	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = malloc(len + 2);
	if (!new)
		exit(1);
	if (*dst)
		ft_memcpy(new, *dst, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(*dst);
	*dst = new;
}

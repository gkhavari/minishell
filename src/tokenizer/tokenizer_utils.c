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

void	mark_word_quoted(t_shell *shell)
{
	shell->word_quoted = 1;
}

void	set_heredoc_mode(t_shell *shell, int mode)
{
	shell->heredoc_mode = mode;
}

int	is_heredoc_mode(t_shell *shell)
{
	return (shell->heredoc_mode);
}

/**
 * Grow *dst and append c; exits on alloc failure.
 */
int	append_char(t_shell *shell, char **dst, char c)
{
	size_t	len;
	char	*new;

	(void)shell;
	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = malloc(len + 2);
	if (!new)
	{
		free(*dst);
		clean_exit(shell, EXIT_FAILURE);
	}
	if (*dst)
		ft_memcpy(new, *dst, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(*dst);
	*dst = new;
	return (SUCCESS);
}

/**
 * If *word non-empty: emit WORD token, clear buffer and quote/heredoc flags.
 */
void	flush_word(t_shell *shell, char **word, t_token **token)
{
	t_token	*tok;

	if (*word)
	{
		tok = new_token(shell, WORD, *word);
		if (!tok)
		{
			free(*word);
			clean_exit(shell, EXIT_FAILURE);
		}
		tok->quoted = shell->word_quoted;
		add_token(token, tok);
		free(*word);
		*word = NULL;
		shell->word_quoted = 0;
		shell->heredoc_mode = 0;
	}
}

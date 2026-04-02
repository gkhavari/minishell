/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 15:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * End tokenization: `flush_word` in ST_NORMAL, or discard partial state in
 * quotes; always free `shell->input`.
 */
static int	tokenizer_end(t_shell *shell, t_state state, char **word)
{
	if (state == ST_NORMAL)
	{
		if (flush_word(shell, word, &shell->tokens) == OOM)
		{
			free_tokenize(shell, word);
			return (OOM);
		}
	}
	else
	{
		msh_strptr_free(word);
		if (shell->tokens)
		{
			free_tokens(&shell->tokens);
			shell->tokens = NULL;
		}
	}
	return (free(shell->input), (shell->input = NULL), SUCCESS);
}

/**
 * Tokenize `shell->input` into `shell->tokens`, then free the line buffer.
 * Unclosed quote clears tokens and sets exit status. Returns OOM if unwound.
 */
int	tokenize_input(t_shell *shell)
{
	t_state	state;
	char	*word;
	size_t	i;
	int		err;

	state = ST_NORMAL;
	word = NULL;
	i = 0;
	err = tokenizer_loop(shell, &i, &state, &word);
	if (err == OOM)
	{
		free_tokenize(shell, &word);
		return (OOM);
	}
	return (tokenizer_end(shell, state, &word));
}

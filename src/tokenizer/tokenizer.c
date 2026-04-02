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

static int	tokenize_finish_end(t_shell *shell, t_state state, char **word)
{
	if (state == ST_NORMAL)
	{
		if (flush_word(shell, word, &shell->tokens) == OOM)
		{
			free_lex(shell, word);
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
	free(shell->input);
	shell->input = NULL;
	return (SUCCESS);
}

/**
 * Lex shell->input into shell->tokens; frees input;
 * clears tokens on quote error. Returns OOM on malloc failure (unwound).
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
	err = tokenizer_run_loop(shell, &i, &state, &word);
	if (err == OOM)
	{
		free_lex(shell, &word);
		return (OOM);
	}
	return (tokenize_finish_end(shell, state, &word));
}

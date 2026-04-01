/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/31 20:15:31 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Quotes, quoted segments, unquoted $ and ~ (respect heredoc mode). */
static int	handle_quotes_and_expand(t_shell *shell, size_t *i,
		char **word, t_state *state)
{
	int	r;

	if (process_quote(shell, shell->input[*i], state))
	{
		if (!*word)
		{
			*word = ft_strdup("");
			if (!*word)
				return (MSH_OOM);
		}
		(*i)++;
		return (1);
	}
	r = handle_single_quote(shell, i, word, state);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r)
		return (1);
	r = handle_double_quote(shell, i, word, state);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r)
		return (1);
	if (!is_heredoc_mode(shell))
	{
		r = handle_variable_expansion(shell, i, word);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			return (1);
	}
	if (!is_heredoc_mode(shell))
	{
		r = handle_tilde_expansion(shell, i, word);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			return (1);
	}
	return (0);
}

/** One pass over shell->input until NUL; dispatches handlers. */
static int	tokenizer_loop(t_shell *shell, size_t *i, t_state *state,
		char **word)
{
	int	r;

	while (1)
	{
		if (!shell->input[*i])
		{
			handle_end_of_string(shell, state, word);
			break ;
		}
		r = handle_quotes_and_expand(shell, i, word, state);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			continue ;
		r = handle_backslash(shell, i, word, state);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			continue ;
		r = handle_operator(shell, i, word);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			continue ;
		r = handle_whitespace(shell, i, word);
		if (r == MSH_OOM)
			return (MSH_OOM);
		if (r)
			continue ;
		r = process_normal_char(shell, shell->input[*i], i, word);
		if (r == MSH_OOM)
			return (MSH_OOM);
	}
	return (SUCCESS);
}

/**
 * Lex shell->input into shell->tokens; frees input;
 * clears tokens on quote error. Returns MSH_OOM on malloc failure (unwound).
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
	if (err == MSH_OOM)
	{
		msh_lex_abort(shell, &word);
		return (MSH_OOM);
	}
	if (state == ST_NORMAL)
	{
		if (flush_word(shell, &word, &shell->tokens) == MSH_OOM)
		{
			msh_lex_abort(shell, &word);
			return (MSH_OOM);
		}
	}
	else
	{
		free(word);
		word = NULL;
		if (shell->tokens)
		{
			free_tokens(shell->tokens);
			shell->tokens = NULL;
		}
	}
	free(shell->input);
	shell->input = NULL;
	return (SUCCESS);
}

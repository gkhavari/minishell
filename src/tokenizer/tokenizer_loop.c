/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_loop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:00:00 by thanh-ng        #+#    #+#             */
/*   Updated: 2026/04/01 15:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Call one tokenizer handler (shell, i, word). Propagate OOM; map handled
 * (non-TOK_N) to TOK_Y; else TOK_N.
 */
static int	tok_call_handler(t_shell *shell, size_t *i, char **word,
		int (*fn)(t_shell *, size_t *, char **))
{
	int	r;

	r = fn(shell, i, word);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (TOK_Y);
	return (TOK_N);
}

static int	tok_try_expand_unquoted(t_shell *shell, size_t *i, char **word)
{
	int	r;

	if (shell->heredoc_mode)
		return (TOK_N);
	r = tok_call_handler(shell, i, word, &handle_variable_expansion);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (TOK_Y);
	return (tok_call_handler(shell, i, word, &handle_tilde_expansion));
}

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
				return (OOM);
		}
		(*i)++;
		return (TOK_Y);
	}
	r = handle_single_quote(shell, i, word, state);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (TOK_Y);
	r = handle_double_quote(shell, i, word, state);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (TOK_Y);
	return (tok_try_expand_unquoted(shell, i, word));
}

static int	tok_run_secondary(t_shell *shell, size_t *i, t_state *state,
		char **word)
{
	int	r;

	r = handle_backslash(shell, i, word, state);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (SUCCESS);
	r = tok_call_handler(shell, i, word, &handle_operator);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (SUCCESS);
	r = tok_call_handler(shell, i, word, &handle_whitespace);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (SUCCESS);
	r = process_normal_char(shell, shell->input[*i], i, word);
	if (r == OOM)
		return (OOM);
	return (SUCCESS);
}

int	tokenizer_loop(t_shell *shell, size_t *i, t_state *state, char **word)
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
		if (r == OOM)
			return (OOM);
		if (r != TOK_N)
			continue ;
		if (tok_run_secondary(shell, i, state, word) == OOM)
			return (OOM);
	}
	return (SUCCESS);
}

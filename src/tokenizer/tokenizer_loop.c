/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_loop.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:22:30 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:22:32 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Call `fn(shell, i, word)`; propagate OOM; if handled return TOK_Y else TOK_N.
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

/**
 * When not tokenizing a heredoc delimiter (`!shell->hd_mod`): try `exp_dollar`
 * then `exp_tilde` at *i.
 */
static int	tok_try_expand_unquoted(t_shell *shell, size_t *i, char **word)
{
	int	r;

	if (shell->hd_mod)
		return (TOK_N);
	r = tok_call_handler(shell, i, word, &exp_dollar);
	if (r == OOM)
		return (OOM);
	if (r != TOK_N)
		return (TOK_Y);
	return (tok_call_handler(shell, i, word, &exp_tilde));
}

/**
 * Quote toggles, single/double-quote handlers, then unquoted $ / ~ expansion.
 */
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

/**
 * Backslash, operators, whitespace, or append literal char (ST_NORMAL path).
 */
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

/**
 * Main per-character loop over `shell->input` until NUL: quotes, expansion,
 * operators.
 */
int	tokenizer_loop(t_shell *shell, size_t *i, t_state *state, char **word)
{
	int	r;

	while (TRUE)
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

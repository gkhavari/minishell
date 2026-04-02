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
 * Run one lexer handler that shares (shell, i, word). Like ft_lstmap-style
 * fn pointer: propagate MSH_OOM; map "handled" to MSH_LEX_YES; else MSH_LEX_NO.
 */
static int	lex_try_one(t_shell *shell, size_t *i, char **word,
		int (*fn)(t_shell *, size_t *, char **))
{
	int	r;

	r = fn(shell, i, word);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (MSH_LEX_YES);
	return (MSH_LEX_NO);
}

static int	lex_try_expand_unquoted(t_shell *shell, size_t *i, char **word)
{
	int	r;

	if (shell->heredoc_mode)
		return (MSH_LEX_NO);
	r = lex_try_one(shell, i, word, &handle_variable_expansion);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (MSH_LEX_YES);
	return (lex_try_one(shell, i, word, &handle_tilde_expansion));
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
				return (MSH_OOM);
		}
		(*i)++;
		return (MSH_LEX_YES);
	}
	r = handle_single_quote(shell, i, word, state);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (MSH_LEX_YES);
	r = handle_double_quote(shell, i, word, state);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (MSH_LEX_YES);
	return (lex_try_expand_unquoted(shell, i, word));
}

static int	lex_run_secondary(t_shell *shell, size_t *i, t_state *state,
		char **word)
{
	int	r;

	r = handle_backslash(shell, i, word, state);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (SUCCESS);
	r = lex_try_one(shell, i, word, &handle_operator);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (SUCCESS);
	r = lex_try_one(shell, i, word, &handle_whitespace);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (SUCCESS);
	r = process_normal_char(shell, shell->input[*i], i, word);
	if (r == MSH_OOM)
		return (MSH_OOM);
	return (SUCCESS);
}

int	tokenizer_run_loop(t_shell *shell, size_t *i, t_state *state, char **word)
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
		if (r != MSH_LEX_NO)
			continue ;
		if (lex_run_secondary(shell, i, state, word) == MSH_OOM)
			return (MSH_OOM);
	}
	return (SUCCESS);
}

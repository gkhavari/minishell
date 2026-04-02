/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_quotes.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/30 20:50:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_dollar_in_dquote(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;

	expanded = expand_var(shell, i);
	if (!expanded)
		return (MSH_OOM);
	if (append_expansion_quoted(word, expanded) == MSH_OOM)
	{
		free(expanded);
		return (MSH_OOM);
	}
	free(expanded);
	return (MSH_LEX_YES);
}

/**
 * In ST_DQUOTE: expand $ (unless heredoc/next quote),
 * handle \\$, or append char.
 */
static int	handle_escaped_dollar(t_shell *shell, size_t *i, char **word)
{
	if (shell->input[*i] != '\\' || shell->input[*i + 1] != '$')
		return (MSH_LEX_NO);
	if (append_char(shell, word, '$') == MSH_OOM)
		return (MSH_OOM);
	*i += 2;
	return (MSH_LEX_YES);
}

/**
 * In ST_DQUOTE: $ / \\$ / literal; else return MSH_LEX_NO.
 */
int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	int	r;

	if (*state != ST_DQUOTE)
		return (MSH_LEX_NO);
	if (shell->input[*i] == '$' && !shell->heredoc_mode
		&& shell->input[*i + 1] != '"' && shell->input[*i + 1] != '\'')
		return (handle_dollar_in_dquote(shell, i, word));
	r = handle_escaped_dollar(shell, i, word);
	if (r == MSH_OOM)
		return (MSH_OOM);
	if (r != MSH_LEX_NO)
		return (MSH_LEX_YES);
	if (process_normal_char(shell, shell->input[*i], i, word) == MSH_OOM)
		return (MSH_OOM);
	return (MSH_LEX_YES);
}

/**
 * In ST_SQUOTE: append char literally (no expansion). Else return MSH_LEX_NO.
 */
int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (MSH_LEX_NO);
	if (process_normal_char(shell, shell->input[*i], i, word) == MSH_OOM)
		return (MSH_OOM);
	return (MSH_LEX_YES);
}

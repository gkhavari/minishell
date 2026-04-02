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
		return (OOM);
	if (append_expansion_quoted(word, expanded) == OOM)
	{
		free(expanded);
		return (OOM);
	}
	free(expanded);
	return (LX_Y);
}

/**
 * In ST_DQUOTE: expand $ (unless heredoc/next quote),
 * handle \\$, or append char.
 */
static int	handle_escaped_dollar(t_shell *shell, size_t *i, char **word)
{
	if (shell->input[*i] != '\\' || shell->input[*i + 1] != '$')
		return (LX_N);
	if (append_char(shell, word, '$') == OOM)
		return (OOM);
	*i += 2;
	return (LX_Y);
}

/**
 * In ST_DQUOTE: $ / \\$ / literal; else return LX_N.
 */
int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	int	r;

	if (*state != ST_DQUOTE)
		return (LX_N);
	if (shell->input[*i] == '$' && !shell->heredoc_mode
		&& shell->input[*i + 1] != '"' && shell->input[*i + 1] != '\'')
		return (handle_dollar_in_dquote(shell, i, word));
	r = handle_escaped_dollar(shell, i, word);
	if (r == OOM)
		return (OOM);
	if (r != LX_N)
		return (LX_Y);
	if (process_normal_char(shell, shell->input[*i], i, word) == OOM)
		return (OOM);
	return (LX_Y);
}

/**
 * In ST_SQUOTE: append char literally (no expansion). Else return LX_N.
 */
int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (LX_N);
	if (process_normal_char(shell, shell->input[*i], i, word) == OOM)
		return (OOM);
	return (LX_Y);
}

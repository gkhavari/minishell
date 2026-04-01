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

/**
 * In ST_SQUOTE: append char literally (no expansion). Else return 0.
 */
int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (0);
	if (process_normal_char(shell, shell->input[*i], i, word) == MSH_OOM)
		return (MSH_OOM);
	return (1);
}

/**
 * In ST_DQUOTE: expand $ (unless heredoc/next quote),
 * handle \\$, or append char.
 */
static int	handle_escaped_dollar(t_shell *shell, size_t *i, char **word)
{
	if (shell->input[*i] != '\\' || shell->input[*i + 1] != '$')
		return (0);
	if (append_char(shell, word, '$') == FAILURE)
	{
		*i += 2;
		return (1);
	}
	*i += 2;
	return (1);
}

/**
 * In ST_DQUOTE: $ / \\$ / literal; else return 0.
 */
int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	char	*expanded;

	if (*state != ST_DQUOTE)
		return (0);
	if (shell->input[*i] == '$' && !is_heredoc_mode(shell)
		&& shell->input[*i + 1] != '"' && shell->input[*i + 1] != '\'')
	{
		expanded = expand_var(shell, i);
		if (!expanded)
		{
			shell->last_exit = FAILURE;
			return (1);
		}
		append_expansion_quoted(word, expanded);
		free(expanded);
		return (1);
	}
	if (handle_escaped_dollar(shell, i, word))
		return (1);
	if (process_normal_char(shell, shell->input[*i], i, word) == MSH_OOM)
		return (MSH_OOM);
	return (1);
}

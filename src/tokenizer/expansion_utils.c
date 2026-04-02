/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:09:05 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:09:12 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * True when the last token is a redirect operator and *word is empty (next arg).
 * Detects ambiguous redirect after `$` expanded to nothing.
 */
static int	exp_redir_tok(t_shell *shell, char *word)
{
	t_list	*last_n;
	t_token	*last;

	if (word)
		return (FALSE);
	last_n = ft_lstlast(shell->tokens);
	if (!last_n)
		return (FALSE);
	last = last_n->content;
	if (last->type == REDIR_IN || last->type == REDIR_OUT
		|| last->type == APPEND || last->type == HEREDOC)
		return (TRUE);
	return (FALSE);
}

/**
 * Push one WORD token whose value is `S_AMBIG` plus the input slice [start, end).
 */
static int	exp_ambg_push(t_shell *shell, size_t start, size_t end)
{
	char	*raw;
	char	*value;
	t_token	*tok;

	raw = ft_strndup(shell->input + start, end - start);
	if (!raw)
		return (OOM);
	value = ft_strjoin(S_AMBIG, raw);
	free(raw);
	if (!value)
		return (OOM);
	tok = new_token(shell, WORD, value);
	free(value);
	if (!tok || add_token(&shell->tokens, tok) == OOM)
		return (OOM);
	return (TOK_Y);
}

/**
 * After `$` expanded to empty: at a word boundary, emit `S_EMPTY` or an
 * ambiguous-redirect WORD. Returns TOK_N if not at boundary, TOK_Y if handled,
 * OOM on failure.
 */
int	exp_empty(t_shell *shell, size_t start, size_t end, char **word)
{
	t_token	*tok;

	if (*word || (shell->input[end] != '\0'
			&& !msh_is_blank((unsigned char)shell->input[end], 1)
			&& !is_op_char(shell->input[end])))
		return (TOK_N);
	if (exp_redir_tok(shell, *word))
		return (exp_ambg_push(shell, start, end));
	tok = new_token(shell, WORD, S_EMPTY);
	if (!tok || add_token(&shell->tokens, tok) == OOM)
		return (OOM);
	return (TOK_Y);
}

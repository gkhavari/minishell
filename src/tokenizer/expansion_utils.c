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
 * True when the last token is a redirect op and *word is empty (next arg).
 * Used to detect ambiguous redirect after empty `$` expansion.
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
 * Push one WORD with S_AMBIG prefix for input slice [start, end) (ambiguous).
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
 * After `$` expanded to empty: at word boundary, emit EMPTY token or ambig.
 * Returns TOK_N if not at boundary, TOK_Y if handled, OOM on failure.
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

/**
 * Append expanded string to *word inside quotes (no blank-based splitting).
 * Returns OOM if ft_realloc fails; else SUCCESS.
 */
int	exp_q_cat(char **word, const char *exp)
{
	size_t	len_word;
	size_t	len_exp;
	char	*tmp;

	if (*word)
		len_word = ft_strlen(*word);
	else
		len_word = 0;
	if (exp)
		len_exp = ft_strlen(exp);
	else
		len_exp = 0;
	tmp = ft_realloc(*word, len_word + len_exp + 1);
	if (!tmp)
		return (OOM);
	*word = tmp;
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
	return (SUCCESS);
}

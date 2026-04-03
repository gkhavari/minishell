/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 22:41:39 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/03 12:22:44 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * If *word non-empty: emit WORD token, clear buffer, word_quoted, and
 * heredoc-delimiter mode (`hd_mod`).
 * Returns OOM if new_token or list append fails.
 */
int	flush_word(t_shell *shell, char **word, t_list **tokens)
{
	t_token	*tok;

	if (*word)
	{
		tok = new_token(shell, WORD, *word);
		if (!tok)
			return (free(*word), (*word = NULL), OOM);
		tok->quoted = shell->word_quoted;
		if (add_token(tokens, tok) == OOM)
			return (free(*word), (*word = NULL), OOM);
		free(*word);
		*word = NULL;
		shell->word_quoted = 0;
		shell->hd_mod = 0;
	}
	return (SUCCESS);
}

/**
 * Allocate token with ft_strdup(value); NULL on failure.
 */
t_token	*new_token(t_shell *shell, t_tokentype type, char *value)
{
	t_token	*token;

	(void)shell;
	token = malloc(sizeof(*token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = ft_strdup(value);
	if (!token->value)
		return (free(token), NULL);
	token->quoted = 0;
	return (token);
}

/**
 * Append new_tok to *head; on list node OOM frees new_tok. SUCCESS or OOM.
 */
int	add_token(t_list **head, t_token *new_tok)
{
	t_list	*node;

	node = ft_lstnew(new_tok);
	if (!node)
		return (free(new_tok->value), free(new_tok), OOM);
	ft_lstadd_back(head, node);
	return (SUCCESS);
}

/**
 * append_char(word, c) then (*i)++. Returns SUCCESS or OOM.
 */
int	process_normal_char(t_shell *shell, char c, size_t *i, char **word)
{
	if (append_char(shell, word, c) == OOM)
		return (OOM);
	(*i)++;
	return (SUCCESS);
}

/**
 * True if c starts an operator (| < >).
 */
int	is_op_char(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (TRUE);
	return (FALSE);
}

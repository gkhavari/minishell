/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 22:41:39 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Grow *dst and append c. On failure frees *dst, sets NULL, returns MSH_OOM.
 */
int	append_char(t_shell *shell, char **dst, char c)
{
	size_t	len;
	char	*new;

	(void)shell;
	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = malloc(len + 2);
	if (!new)
	{
		free(*dst);
		*dst = NULL;
		return (MSH_OOM);
	}
	if (*dst)
		ft_memcpy(new, *dst, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(*dst);
	*dst = new;
	return (SUCCESS);
}

/**
 * If *word non-empty: emit WORD token, clear buffer and quote/heredoc flags.
 * Returns MSH_OOM if new_token or list append fails.
 */
int	flush_word(t_shell *shell, char **word, t_list **tokens)
{
	t_token	*tok;

	if (*word)
	{
		tok = new_token(shell, WORD, *word);
		if (!tok)
		{
			free(*word);
			*word = NULL;
			return (MSH_OOM);
		}
		tok->quoted = shell->word_quoted;
		if (add_token(tokens, tok) == MSH_OOM)
		{
			free(*word);
			*word = NULL;
			return (MSH_OOM);
		}
		free(*word);
		*word = NULL;
		shell->word_quoted = 0;
		shell->heredoc_mode = 0;
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
	{
		free(token);
		return (NULL);
	}
	token->quoted = 0;
	return (token);
}

/*
** Append t_token * in a new list node at end of *head.
** Returns SUCCESS or MSH_OOM.
*/
int	add_token(t_list **head, t_token *new_tok)
{
	t_list	*node;

	node = ft_lstnew(new_tok);
	if (!node)
	{
		free(new_tok->value);
		free(new_tok);
		return (MSH_OOM);
	}
	ft_lstadd_back(head, node);
	return (SUCCESS);
}

/**
 * append_char(word, c) then (*i)++. Returns SUCCESS or MSH_OOM.
 */
int	process_normal_char(t_shell *shell, char c, size_t *i, char **word)
{
	if (append_char(shell, word, c) == MSH_OOM)
		return (MSH_OOM);
	(*i)++;
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 14:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	token->next = NULL;
	return (token);
}

/** Append new at end of singly-linked token list. */
void	add_token(t_token **head, t_token *new)
{
	t_token	*tmp;

	if (!(*head))
		*head = new;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

/**
 * append_char(word, c) then (*i)++. Returns SUCCESS or FAILURE.
 */
int	process_normal_char(t_shell *shell, char c, size_t *i, char **word)
{
	if (append_char(shell, word, c) == FAILURE)
	{
		(*i)++;
		return (FAILURE);
	}
	(*i)++;
	return (SUCCESS);
}

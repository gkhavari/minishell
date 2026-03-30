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
 DESCRIPTION:
 * Creates and initializes a new token.
 * This allocates memory for a token structure, sets its type, duplicates its 
 	string value, and initializes its next pointer to NULL.
 
 PARAMETERS:
 * type: The token type (e.g., WORD, PIPE, REDIR_IN, etc.).
 * value: The string value associated with the token. 
 	This is duplicated internally.

RETURN VALUE:
 * A pointer to the newly created token.
 * NULL if memory allocation fails.
**/
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
 DESCRIPTION:
 * Appends a single character to the current word buffer and advances 
 	the input index.

 PARAMETERS:
 * char c: The character to append to the current word buffer.
 * size_t *i: Pointer to the current index in the input string. 
 	This index is incremented after the character is processed.
 * char **word: Pointer to the current word buffer. The character is 
 	appended to this buffer.

 BEHAVIOR:
 * Calls append_char(word, c) to append the character to the word buffer.
 * Increments *i to move to the next character in the input.
 **/
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

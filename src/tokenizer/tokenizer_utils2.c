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

	token = msh_calloc(shell, 1, sizeof(*token));
	token->type = type;
	token->value = ft_strdup(value);
	token->next = NULL;
	return (token);
}

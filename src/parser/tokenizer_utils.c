/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 22:41:39 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 22:41:41 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
 * Appends a single character to the end of a dynamically allocated string.
 * This function grows the buffer by allocating a new one, copying the 
 	existing data, adding the new character, and replacing the old pointer. 
	This is used during tokenization to gradually build words one character 
	at a time.

 PARAMETERS:
 * dst: Pointer to a dynamically allocated string. If *dst is NULL, a new 
 	1-character string is created.
 * c: The character to append.

 RETURN VALUE:
 * None.
 * The buffer referenced by *dst is replaced with an enlarged version 
 	containing the new character.
 * The original *dest is freed.
 * resulting string is null-terminated.
**/
void	append_char(char **dst, char c)
{
	size_t	len;
	char	*new;

	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = malloc(len + 2);
	if (!new)
		exit(1);
	if (*dst)
		ft_memcpy(new, *dst, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(*dst);
	*dst = new;
}

/** 
 DESCRIPTION:
 * Appends a token to the end of a linked list of tokens.
 * If the list is empty, the new token becomes the head.
 * Otherwise, the function walks to the end of the list and inserts the 
 	new token.

 PARAMETERS:
 * head: Pointer to the head pointer of the token list.
 * new: The token to append.

 RETURN VALUE:
 * None.
 **/
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
t_token	*new_token(t_tokentype type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(*token));
	if (!token)
		return (NULL); //todo: error handling
	token->type = type;
	token->value = ft_strdup(value);
	token->next = NULL;
	return (token);
}

/**
 DESCRIPTION:
 * Finalizes the current accumulated word and converts it into a WORD token.
 * If a partial word is being built (via append_char), this function:
 ** Wraps it into a new token.
 ** Adds the token to the token list.
 ** Frees the word buffer.
 ** Resets the pointer to NULL.

 PARAMETERS:
 * word: pointer to the buffer, storing the current built word.
 * token: pointer to the tokenlist, where the word token will be appended

 RETURN VALUE:
 * none
**/
void	flush_word(char **word, t_token **token)
{
	if (*word)
	{
		add_token(token, new_token(WORD, *word));
		free(*word);
		*word = NULL;
	}
}

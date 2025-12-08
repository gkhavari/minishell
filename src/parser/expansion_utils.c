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
 DESCRIPTION:
* Appends a string exp to an existing word buffer *word

 BEHAVIOR:
* Calculates the current length of *word and the length of exp.
* Reallocates *word to accommodate the new string.
* Copies exp to the end of *word.
* Ensures the final string is null-terminated.

 PARAMETERS:
* char **word: Pointer to the current word buffer. Can be NULL if no content
	has been added yet. The buffer will be reallocated to append exp.
* const char *exp: The string to append. Can be NULL, in which case 
	the function does nothing.
**/
void	append_expansion_quoted(char **word, const char *exp)
{
	size_t	len_word;
	size_t	len_exp;

	if (*word)
		len_word = ft_strlen(*word);
	else
		len_word = 0;
	if (exp)
		len_exp = ft_strlen(exp);
	else
		len_exp = 0;
	*word = realloc(*word, len_word + len_exp + 1); //change to allowed function
	if (!*word)
		exit(1);
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
}

/**
 DESCRIPTION:
 * Appends a string exp to a word buffer *word while splitting the content
 	on whitespace

 BEHAVOIR:
 * Iterates through each character of exp.
 * If a whitespace character is found:
 ** Flushes the current word to the token list (if any).
 ** Skips over consecutive whitespace.
 * If a non-whitespace character is found:
 ** Appends it to *word.
 * Continues until the entire expansion is processed.

 PARAMETERS:
 * char **word: Pointer to the current word buffer being constructed.
 	Can be NULL.
 * const char *exp: The string to append. If NULL, the function does nothing.
 * t_token **tokens: Pointer to the head of the token list. Complete words are 
 	flushed to this list when whitespace is encountered.
 */
void	append_expansion_unquoted(char **word, const char *exp,
		t_token **tokens)
{
	size_t	i;

	i = 0;
	if (exp == NULL)
		return ;
	while (exp[i])
	{
		if (isspace(exp[i]))
		{
			if (*word)
				flush_word(word, tokens);
			while (isspace(exp[i]))
				i++;
		}
		else
			process_normal_char(exp[i], &i, word);
	}
}

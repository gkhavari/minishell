/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:09:05 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 18:11:20 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Append a quoted expansion string to a word buffer.

 BEHAVIOR:
* Computes the current length of `*word` and the length of `exp`,
* reallocates `*word` to hold the new combined contents, copies `exp`
* to the end of the buffer and ensures the result is NUL-terminated.
* Exits on allocation failure.

 PARAMETERS:
* char **word: Pointer to the current word buffer; may be NULL.
* const char *exp: Expansion string to append; may be NULL.
*/
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
	*word = ft_realloc(*word, len_word + len_exp + 1);
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
void	append_expansion_unquoted(t_shell *shell, char **word, const char *exp,
		t_token **tokens)
{
	size_t	i;

	i = 0;
	if (exp == NULL)
		return ;
	while (exp[i])
	{
		if (exp[i] == ' ' || exp[i] == '\t')
		{
			if (*word)
				flush_word(shell, word, tokens);
			while (exp[i] == ' ' || exp[i] == '\t')
				i++;
		}
		else
			process_normal_char(shell, exp[i], &i, word);
	}
}

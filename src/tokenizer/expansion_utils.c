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

static int	is_word_boundary(char c)
{
	if (c == '\0' || c == ' ' || c == '\t')
		return (1);
	return (is_op_char(c));
}

static int	is_redir_target(t_shell *shell, char *word)
{
	t_token	*last;

	if (word)
		return (0);
	last = shell->tokens;
	while (last && last->next)
		last = last->next;
	if (!last)
		return (0);
	return (last->type == REDIR_IN || last->type == REDIR_OUT
		|| last->type == APPEND || last->type == HEREDOC
	);
}

int	handle_empty_unquoted_expansion(t_shell *shell, size_t start,
		size_t end, char **word)
{
	char	*raw;
	char	*value;

	if (*word || !is_word_boundary(shell->input[end]))
		return (0);
	if (!is_redir_target(shell, *word))
		return (add_token(&shell->tokens,
				new_token(shell, WORD, MSH_EMPTY_EXPAND_TOKEN)), 1);
	raw = ft_strndup(shell->input + start, end - start);
	if (!raw)
		return (1);
	value = ft_strjoin(MSH_AMBIG_REDIR_PREFIX, raw);
	free(raw);
	if (!value)
		return (1);
	add_token(&shell->tokens, new_token(shell, WORD, value));
	free(value);
	return (1);
}

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
		return ;
	*word = tmp;
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

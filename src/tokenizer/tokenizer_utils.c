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

static int	g_word_quoted;
static int	g_heredoc_mode;

void	mark_word_quoted(void)
{
	g_word_quoted = 1;
}

void	set_heredoc_mode(int mode)
{
	g_heredoc_mode = mode;
}

int	is_heredoc_mode(void)
{
	return (g_heredoc_mode);
}

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
void	append_char(t_shell *shell, char **dst, char c)
{
	size_t	len;
	char	*new;

	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = msh_calloc(shell, sizeof(char), len + 2);
	if (*dst)
		ft_memcpy(new, *dst, len);
	new[len] = c;
	new[len + 1] = '\0';
	free(*dst);
	*dst = new;
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
 * shell: pointer to the shell (contains tokenlist to append to).
 * word: pointer to the buffer storing the current built word.
 * token: pointer to the tokenlist where the word token is appended

 RETURN VALUE:
 * none
**/
void	flush_word(t_shell *shell, char **word, t_token **token)
{
	t_token	*tok;

	if (*word)
	{
		tok = new_token(shell, WORD, *word);
		tok->quoted = g_word_quoted;
		add_token(token, tok);
		free(*word);
		*word = NULL;
	}
	g_word_quoted = 0;
	g_heredoc_mode = 0;
}

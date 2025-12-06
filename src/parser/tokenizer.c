/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 22:41:31 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
 * Handles transitions between quoting states in the tokenizer.
 * This function detects opening and closing of:
 ** single quotes '...'
 ** double quotes "..."
	It updates the parsing state accordingly.

 PARAMETERS:
 * c: the current character
 * state: Pointer to the state variable
 
 RETURN VALUE:
 * 1 if the character was a quote that changed the state.
 * 0 otherwise
**/
static int	process_quote(char c, t_state *state)
{
	if (*state == ST_NORMAL && c == '\'')
	{
		*state = ST_SQUOTE;
		return (1);
	}
	if (*state == ST_SQUOTE && c == '\'')
	{
		*state = ST_NORMAL;
		return (1);
	}
	if (*state == ST_NORMAL && c == '"')
	{
		*state = ST_DQUOTE;
		return (1);
	}
	if (*state == ST_DQUOTE && c == '"')
	{
		*state = ST_NORMAL;
		return (1);
	}
	return (0);
}

/**
 DESCRIPTION:
 * Handles whitespace during tokenization.
 * When a space is encountered:
 ** Finalizes the current word (if any).
 ** Advances the input index.
 ** Produces no token for the whitespace.

 PARAMETERS:
 * s: The input string.
 * i: Pointer to the current index.
 * word: Pointer to the accumulating word buffer.
 * tokens: Pointer to the token list.

 RETURN VALUE:
 * 1 if whitespace was processed.
 * 0 otherwise.
 **/
static int	handle_whitespace(const char *s, size_t *i, char **word,
	t_token **tokens)
{
	if (isspace(s[*i])) //todo: add this function to libft.
	{
		flush_word(word, tokens);
		(*i)++;
		return (1);
	}
	return (0);
}

/**
 DESCRIPTION:
 * Main tokenizer function.
 * Converts the raw input line into a linked list of tokens.
 * It processes:
 ** quoted strings
 ** operators (|, <, >, <<, >>)
 ** whitespace
 ** normal characters
 * The function loops through the input and delegates tasks to 
 	helper routines such as:
 ** process_quote()
 ** handle_operator()
 ** handle_whitespace()
 ** flush_word()

 PARAMETERS:
 * shell: Pointer to the shell struct containing input and output 
 	token list.

 RETURN VALUE:
 * None.
 * The result is stored in shell->tokens.
 **/
void	tokenize_input(t_shell *shell)
{
	char	*s;
	t_state	state;
	char	*word;
	size_t	i;

	s = shell->input;
	state = ST_NORMAL;
	word = NULL;
	i = 0;
	shell->tokens = NULL;
	while (1)
	{
		if (!s[i])
		{
			if (state == ST_SQUOTE || state == ST_DQUOTE)
			{
				if (!append_continuation(&s, state))
                    break ;
				continue ;
			}
			else
			{
				break;
			}
		}
		if (process_quote(s[i], &state))
		{
			i++;
			continue ;
		}
		if (state == ST_SQUOTE || state == ST_DQUOTE)
		{
			append_char(&word, s[i]);
			i++;
			continue ;
		}
		if (handle_operator(s, &i, &word, &shell->tokens))
			continue ;
		if (handle_whitespace(s, &i, &word, &shell->tokens))
			continue ;
		append_char(&word, s[i]);
		i++;
	}
	flush_word(&word, &shell->tokens);
	free(s);
	shell->input = NULL;
}

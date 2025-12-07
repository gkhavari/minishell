/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:01 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:01:03 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_end_of_string(t_shell *shell, size_t *i,	t_state *state)
{
	if (*state == ST_SQUOTE || *state == ST_DQUOTE)
	{
		if (!append_continuation(&shell->input, *state))
			return (0);
		*i = 0;
		return (1);
	}
	else
	{
		add_history(shell->input);
		return (0);
	}
}

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
int	process_quote(char c, t_state *state)
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
 * Handles detection and processing of operators during tokenization.
 ** Checks if the current character starts an operator.
 ** Flushes any partially built word.
 ** Reads the operator and creates the corresponding token.
 ** Advances the input index accordingly.

 PARAMETERS:
 * s: the input string
 * i: Pointer to the current index in the string.
 * word: Pointer to the current accumulating word.
 * tokens: Pointer to the token list.

 RETURN VALUE:
 * 1 if an operator was processed.
 * 0 otherwise.
**/
int	handle_operator(t_shell *shell, size_t *i, char **word)
{
	if (is_op_char(shell->input[*i]))
	{
		flush_word(word, &shell->tokens);
		*i += read_operator(&shell->input[*i], &shell->tokens);
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
int	handle_whitespace(t_shell *shell, size_t *i, char **word)
{
	if (isspace(shell->input[*i])) //todo: add this function to libft.
	{
		flush_word(word, &shell->tokens);
		(*i)++;
		return (1);
	}
	return (0);
}

void	process_normal_char(char c, size_t *i, char **word)
{
	append_char(word, c);
	(*i)++;
}

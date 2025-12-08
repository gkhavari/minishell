/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:00:50 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
 * Main internal loop for tokenizing the shell input string.
 * Iterates character by character over shell->input, identifies quotes,
 	variable expansions, operators, whitespace, and normal characters, 
	and builds tokens accordingly.

 PARAMETERS:
 * t_shell *shell: Pointer to the shell structure, containing the input string 
 	and the token list.
 * size_t *i: Pointer to the current index in shell->input. The function updates 
 	this as it progresses.
 * t_state *state: Pointer to the current parser state,
 	used to handle context-sensitive parsing.
 * char **word: Pointer to the current word buffer being built. Tokens are
 	constructed by appending characters to this buffer and eventually flushing
	it to the shell’s token list.

BEHAVIOR:
* Iterates through the input string until the end.
* Checks the current character and handles it based on context:
** End of string: Calls handle_end_of_string().
** Quotes: Handles both single ' and double " quotes. Updates state accordingly.
** Variable expansions: Expands shell variables when $ is encountered.
** Operators: Detects shell operators like |, <, >, etc.
** Whitespace: Flushes completed words as tokens when encountering
	spaces or tabs.
** Normal characters: Appends characters to the current word buffer.
* Continues looping until all characters are processed.
 */
static void	tokenizer_loop(t_shell *shell, size_t *i, t_state *state,
		char **word)
{
	while (1)
	{
		if (!shell->input[*i])
		{
			if (handle_end_of_string(shell, state))
				continue ;
			break ;
		}
		if (process_quote(shell->input[*i], state))
		{
			(*i)++;
			continue ;
		}
		if (handle_single_quote(shell, i, word, state))
			continue ;
		if (handle_double_quote(shell, i, word, state))
			continue ;
		if (handle_variable_expansion(shell, i, word))
			continue ;
		if (handle_operator(shell, i, word))
			continue ;
		if (handle_whitespace(shell, i, word))
			continue ;
		process_normal_char(shell->input[*i], i, word);
	}
}

/**
 DESCRIPTION:
 * Top-level function to tokenize the input line of the shell.

 BEHAVIOR:
 * Initializes the parser state (ST_NORMAL) and the word buffer (word = NULL).
 * Sets the starting index i = 0.
 * Clears any previous tokens in shell->tokens.
 * Calls tokenizer_loop() to iterate over the input and build tokens.
 * Flushes any remaining word in the buffer to the token list.
 * Frees the original input string and sets shell->input to NULL.
 
 PARAMETERS:
 * t_shell *shell: Pointer to the shell structure containing the 
 	input line (shell->input) and where the resulting tokens (shell->tokens) 
	will be stored.
 */
void	tokenize_input(t_shell *shell)
{
	t_state	state;
	char	*word;
	size_t	i;

	state = ST_NORMAL;
	word = NULL;
	i = 0;
	shell->tokens = NULL;
	tokenizer_loop(shell, &i, &state, &word);
	flush_word(&word, &shell->tokens);
	free(shell->input);
	shell->input = NULL;
}

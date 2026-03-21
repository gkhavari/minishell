/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:18:56 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 DESCRIPTION:
* Handle quoted sequences and expansions encountered during tokenization.

 BEHAVIOR:
* Processes quotes and variable/tilde expansions. If a quote or expansion
* was handled this function advances the input index and returns 1. Returns
* 0 when no special handling occurred.

 PARAMETERS:
* t_shell *shell: Shell runtime containing the input.
* size_t *i: Pointer to the current input index.
* char **word: Pointer to the current word buffer being constructed.
* t_state *state: Current tokenizer state (normal, in-quote, etc.).

 RETURN:
* 1 if input was handled, 0 otherwise.
*/
static int	handle_quotes_and_expand(t_shell *shell, size_t *i,
		char **word, t_state *state)
{
	if (process_quote(shell, shell->input[*i], state))
	{
		if (!*word)
			*word = ft_strdup("");
		(*i)++;
		return (1);
	}
	if (handle_single_quote(shell, i, word, state))
		return (1);
	if (handle_double_quote(shell, i, word, state))
		return (1);
	if (!is_heredoc_mode(shell) && handle_variable_expansion(shell, i, word))
		return (1);
	if (!is_heredoc_mode(shell) && handle_tilde_expansion(shell, i, word))
		return (1);
	return (0);
}

/**
 DESCRIPTION:
* Main tokenizer loop that drives character-by-character processing.

 BEHAVIOR:
* Iterates over `shell->input`, dispatching to handlers for quotes,
* backslashes, operators, whitespace and normal characters. Builds tokens
* into `shell->tokens` and exits at end-of-input.

 PARAMETERS:
* t_shell *shell: Shell runtime containing the input and token list.
* size_t *i: Pointer to the current input index.
* t_state *state: Pointer to tokenizer state.
* char **word: Pointer to the current word buffer being built.
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
		if (handle_quotes_and_expand(shell, i, word, state))
			continue ;
		if (handle_backslash(shell, i, word, state))
			continue ;
		if (handle_operator(shell, i, word))
			continue ;
		if (handle_whitespace(shell, i, word))
			continue ;
		process_normal_char(shell, shell->input[*i], i, word);
	}
}

/**
 DESCRIPTION:
* Tokenize the contents of `shell->input` into a linked list of tokens.

 BEHAVIOR:
* Initializes tokenizer state, runs the tokenizer loop, flushes any
* partially built word into `shell->tokens`, frees `shell->input` and
* resets it to NULL.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `input` and `tokens`.
*/
void	tokenize_input(t_shell *shell)
{
	t_state	state;
	char	*word;
	size_t	i;

	state = ST_NORMAL;
	word = NULL;
	i = 0;
	tokenizer_loop(shell, &i, &state, &word);
	flush_word(shell, &word, &shell->tokens);
	free(shell->input);
	shell->input = NULL;
}

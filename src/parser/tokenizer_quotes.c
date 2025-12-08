/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_quotes.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:01:16 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Handles a character in the input when the character is inside a single-quoted 
	string ('...'). In single quotes, all characters are treated literally—no 
	variable expansion occurs.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure, containing the input string.
* size_t *i: Pointer to the current index in the input string.
* char **word: Pointer to the current word buffer being built.
* t_state *state Pointer to the current state. This function only acts
	if the state is ST_SQUOTE.

BEHAVIOR:
* Checks if the current state is ST_SQUOTE.
* If not, returns 0 to indicate it did not handle the character.
* If inside single quotes, appends the current character to the word buffer 
	using process_normal_char().
* Returns 1 to indicate the character was handled.
 **/
int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (0);
	process_normal_char(shell->input[*i], i, word);
	return (1);
}

/**
 DESCRIPTION:
* Handles a character in the input when the character is inside a double-quoted
	string ("..."). Unlike single quotes, double quotes allow variable expansion
	($VAR) and escape sequences.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure, containing the input string.
* size_t *i: Pointer to the current index in the input string.
* char **word: Pointer to the current word buffer being built.
* t_state *state: Pointer to the current state. This function only acts
	if the state is ST_DQUOTE.

BEHAVOIR:
* Checks if the state is in ST_DQUOTE.
* If not, returns 0 to indicate it did not handle the character.
* If the current character is $:
** Calls expand_var() to get the variable’s value.
** Appends the expansion to the word using append_expansion_quoted().
** Frees the temporary expansion buffer.
** Returns 1 to indicate the character was handled.
* If the current character is \$ (escaped $):
** Appends a literal $ to the word buffer.
** Advances the index by 2 to skip the escape sequence.
** Returns 1 to indicate the character was handled.
* Otherwise, appends the character literally via process_normal_char().
* Returns 1 to indicate the character was handled.
 **/
int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	char	*expanded;

	if (*state != ST_DQUOTE)
		return (0);
	if (shell->input[*i] == '$')
	{
		expanded = expand_var(shell, i);
		append_expansion_quoted(word, expanded);
		free(expanded);
		return (1);
	}
	if (shell->input[*i] == '\\' && shell->input[*i + 1] == '$')
	{
		append_char(word, '$');
		*i += 2;
		return (1);
	}
	process_normal_char(shell->input[*i], i, word);
	return (1);
}

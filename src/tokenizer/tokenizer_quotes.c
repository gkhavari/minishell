/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_quotes.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:26:28 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 * DESCRIPTION:
 * Handle a character when inside a double-quoted string. Double quotes allow
 * variable expansion and certain escape sequences while preserving most
 * characters literally.
 *
 * PARAMETERS:
 * t_shell *shell: Shell runtime containing the input string.
 * size_t *i: Pointer to the current index in the input string.
 * char **word: Pointer to the current word buffer being built.
 * t_state *state: Pointer to the current tokenizer state; this function acts
 *  only when state == ST_DQUOTE.
 *
 * BEHAVIOR:
 * If the current character is `$` and expansion is permitted, call
 * `expand_var()` and append the result via `append_expansion_quoted()`.
 * If the current character is an escaped `$` append a literal `$` and
 * advance the input index accordingly.
 * Otherwise append the character literally via `process_normal_char()`.
 * Returns 1 if the character was handled, 0 otherwise.
 */
int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	char	*expanded;

	if (*state != ST_DQUOTE)
		return (0);
	if (shell->input[*i] == '$' && !is_heredoc_mode(shell)
		&& shell->input[*i + 1] != '"'
		&& shell->input[*i + 1] != '\'')
	{
		expanded = expand_var(shell, i);
		append_expansion_quoted(word, expanded);
		free(expanded);
		return (1);
	}
	if (shell->input[*i] == '\\' && shell->input[*i + 1] == '$')
	{
		append_char(shell, word, '$');
		*i += 2;
		return (1);
	}
	process_normal_char(shell, shell->input[*i], i, word);
	return (1);
}

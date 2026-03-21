/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:01 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:18:29 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Handles the situation when the tokenizer reaches the end of the input string.
	Depending on the current parser state, it either requests input continuation
	for unfinished quotes or finalizes the input by adding it to the
	shell history.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure containing the input string
	and history.
* t_state *state: Pointer to the current parser state (ST_NORMAL, ST_SQUOTE,
	or ST_DQUOTE).

BEHAVIOR:
* If the parser is inside a quote (ST_SQUOTE or ST_DQUOTE):
** Calls append_continuation(&shell->input, *state) to allow the user to continue
	the input line (multi-line command).
** Returns 1 to indicate that the end of string handling requires continuation.
* If the parser is not inside a quote:
** Adds the input to shell history using add_history(shell->input).
** Returns 0 to indicate that no further processing is required.
**/
int	handle_end_of_string(t_shell *shell, t_state *state)
{
	if (*state == ST_SQUOTE || *state == ST_DQUOTE)
	{
		if (!append_continuation(shell, &shell->input, *state))
			return (0);
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
 * Handles backslash escape sequences in the NORMAL (unquoted) tokenizer state.
 * In unquoted context, a backslash followed by any character means "treat
 * the next character literally" (no expansion, no operator, no word split).
 * This matches POSIX/bash: echo \$USER prints $USER literally.

 PARAMETERS:
 * shell: shell struct (for append_char)
 * i: current index in input string (advanced by 2 on match)
 * word: accumulating word buffer
 * state: current tokenizer state (only active in ST_NORMAL)

 RETURN VALUE:
 * 1 if backslash was handled, 0 otherwise.
**/
/* Skip the backslash, append next char literally */
int	handle_backslash(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (shell->input[*i] != '\\' || *state != ST_NORMAL)
		return (0);
	if (!shell->input[*i + 1])
	{
		(*i)++;
		return (1);
	}
	if (!*word)
		*word = ft_strdup("");
	append_char(shell, word, shell->input[*i + 1]);
	*i += 2;
	return (1);
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
int	process_quote(t_shell *shell, char c, t_state *state)
{
	if (*state == ST_NORMAL && c == '\'')
	{
		*state = ST_SQUOTE;
		mark_word_quoted(shell);
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
		mark_word_quoted(shell);
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
	if (!is_op_char(shell->input[*i]))
		return (0);
	if (*word && !(*word)[1] && (*word)[0] == '2' && shell->input[*i] == '>')
	{
		free(*word);
		*word = NULL;
		add_token(&shell->tokens, new_token(shell, REDIR_ERR_OUT, "2>"));
		(*i)++;
		return (1);
	}
	flush_word(shell, word, &shell->tokens);
	if (shell->input[*i] == '<' && shell->input[*i + 1] == '<')
		set_heredoc_mode(shell, 1);
	*i += read_operator(shell, &shell->input[*i], &shell->tokens);
	return (1);
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
	if (shell->input[*i] == ' ' || shell->input[*i] == '\t')
	{
		flush_word(shell, word, &shell->tokens);
		(*i)++;
		return (1);
	}
	return (0);
}

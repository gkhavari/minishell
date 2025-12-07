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
static int	handle_whitespace(t_shell *shell, size_t *i, char **word)
{
	if (isspace(shell->input[*i])) //todo: add this function to libft.
	{
		flush_word(word, &shell->tokens);
		(*i)++;
		return (1);
	}
	return (0);
}

static int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;

	if (shell->input[*i] != '$')
		return (0);
	expanded = expand_var(shell->input, i, shell);
	append_expansion_unquoted(word, expanded, &shell->tokens);
	free(expanded);
	return (1);
}

static int	handle_double_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	char	*expanded;

	if (*state != ST_DQUOTE)
		return (0);
	if (shell->input[*i] == '$')
	{
		expanded = expand_var(shell->input, i, shell);
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
	append_char(word, shell->input[*i]);
	(*i)++;
	return (1);
}

static int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (0);
	append_char(word, shell->input[*i]);
	(*i)++;
	return (1);
}

static int	handle_end_of_string(t_shell *shell, size_t *i,	t_state *state)
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

static void	process_normal_char(t_shell *shell, size_t *i, char **word)
{
	append_char(word, shell->input[*i]);
	(*i)++;
}

static void	tokenizer_loop(t_shell *shell, size_t *i, t_state *state, char **word)
{
	while (1)
	{
		if (!shell->input[*i])
		{
			if (handle_end_of_string(shell, i, state))
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
		process_normal_char(shell, i, word);
	}
}

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

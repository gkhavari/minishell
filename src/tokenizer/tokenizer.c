/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/31 20:15:31 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_quotes_and_expand(t_shell *shell, size_t *i,
		char **word, t_state *state)
{
	if (process_quote(shell, shell->input[*i], state))
	{
		if (!*word)
		{
			*word = ft_strdup("");
			if (!*word)
				shell->last_exit = 1;
		}
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

static void	tokenizer_loop(t_shell *shell, size_t *i, t_state *state,
		char **word)
{
	while (1)
	{
		if (!shell->input[*i])
		{
			handle_end_of_string(shell, state, word);
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

void	tokenize_input(t_shell *shell)
{
	t_state	state;
	char	*word;
	size_t	i;

	state = ST_NORMAL;
	word = NULL;
	i = 0;
	tokenizer_loop(shell, &i, &state, &word);
	if (state == ST_NORMAL)
		flush_word(shell, &word, &shell->tokens);
	else if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	free(shell->input);
	shell->input = NULL;
}

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

static void	tokenizer_loop(t_shell *shell, size_t *i, t_state *state,
		char **word)
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
		process_normal_char(shell->input[*i], i, word);
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

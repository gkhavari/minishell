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

int	handle_single_quote(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (*state != ST_SQUOTE)
		return (0);
	append_char(word, shell->input[*i]);
	(*i)++;
	return (1);
}

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
	append_char(word, shell->input[*i]);
	(*i)++;
	return (1);
}

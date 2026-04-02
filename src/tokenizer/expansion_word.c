/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_word.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 12:00:00 by thanh-ng        #+#    #+#             */
/*   Updated: 2026/04/01 12:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Append exp to *word; flush words on spaces/tabs into tokens.
 * Returns OOM on allocation failure.
 */
int	append_expansion_unquoted(t_shell *shell, char **word, const char *exp,
		t_list **tokens)
{
	size_t	i;

	i = 0;
	if (exp == NULL)
		return (SUCCESS);
	while (exp[i])
	{
		if (msh_is_blank((unsigned char)exp[i], 1))
		{
			if (*word && flush_word(shell, word, tokens) == OOM)
				return (OOM);
			while (exp[i] && msh_is_blank((unsigned char)exp[i], 1))
				i++;
		}
		else
		{
			if (process_normal_char(shell, exp[i], &i, word) == OOM)
				return (OOM);
		}
	}
	return (SUCCESS);
}

static int	append_expanded_unquoted(t_shell *shell, char **word, char *exp)
{
	if (append_expansion_unquoted(shell, word, exp, &shell->tokens) == OOM)
	{
		free(exp);
		return (OOM);
	}
	free(exp);
	return (LX_Y);
}

int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;
	size_t	start;
	int		he;

	if (shell->input[*i] != '$')
		return (LX_N);
	start = *i;
	expanded = expand_var(shell, i);
	if (!expanded)
		return (OOM);
	if (expanded[0] != '\0')
		return (append_expanded_unquoted(shell, word, expanded));
	he = handle_empty_unquoted_expansion(shell, start, *i, word);
	if (he == OOM)
	{
		free(expanded);
		return (OOM);
	}
	if (he != LX_N)
	{
		free(expanded);
		return (LX_Y);
	}
	return (append_expanded_unquoted(shell, word, expanded));
}

int	handle_tilde_expansion(t_shell *shell, size_t *i, char **word)
{
	char	next;
	char	*home;

	if (shell->input[*i] != '~' || *word)
		return (LX_N);
	next = shell->input[*i + 1];
	if (next && next != '/' && !msh_is_blank((unsigned char)next, 0)
		&& !is_op_char(next))
		return (LX_N);
	home = get_env_value(shell->envp, "HOME");
	if (!home)
		home = "";
	if (append_expansion_unquoted(shell, word, home, &shell->tokens)
		== OOM)
		return (OOM);
	(*i)++;
	return (LX_Y);
}

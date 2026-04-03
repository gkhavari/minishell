/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_word.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:22:19 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:22:21 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Append expanded text in unquoted context: blanks split into new WORD tokens.
 * Returns OOM on allocation failure; SUCCESS otherwise.
 */
int	exp_unq(t_shell *shell, char **word, const char *exp, t_list **tokens)
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

/**
 * Run `exp_unq` into `shell->tokens`, then `free(exp)`. Returns TOK_Y or OOM.
 */
static int	exp_unq_wrap(t_shell *shell, char **word, char *exp)
{
	if (exp_unq(shell, word, exp, &shell->tokens) == OOM)
		return (free(exp), OOM);
	return (free(exp), TOK_Y);
}

/**
 * If *i is `$`, expand with `exp_var`; merge into *word / tokens or handle
 * empty expansion via `exp_empty`. Returns TOK_N, TOK_Y, or OOM.
 */
int	exp_dollar(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;
	size_t	start;
	int		he;

	if (shell->input[*i] != '$')
		return (TOK_N);
	start = *i;
	expanded = exp_var(shell, i);
	if (!expanded)
		return (OOM);
	if (expanded[0] != '\0')
		return (exp_unq_wrap(shell, word, expanded));
	he = exp_empty(shell, start, *i, word);
	if (he == OOM)
		return (free(expanded), OOM);
	if (he != TOK_N)
		return (free(expanded), TOK_Y);
	return (exp_unq_wrap(shell, word, expanded));
}

/**
 * If *i is `~` at word start and HOME applies, append HOME and advance *i.
 * Returns TOK_N when not applicable, TOK_Y when expanded, OOM on failure.
 */
int	exp_tilde(t_shell *shell, size_t *i, char **word)
{
	char	next;
	char	*home;

	if (shell->input[*i] != '~' || *word)
		return (TOK_N);
	next = shell->input[*i + 1];
	if (next && next != '/' && !msh_is_blank((unsigned char)next, 0)
		&& !is_op_char(next))
		return (TOK_N);
	home = get_env_value(shell->envp, "HOME");
	if (!home)
		home = "";
	if (exp_unq(shell, word, home, &shell->tokens) == OOM)
		return (OOM);
	(*i)++;
	return (TOK_Y);
}

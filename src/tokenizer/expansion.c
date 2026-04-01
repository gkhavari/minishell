/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 20:01:07 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/06 20:01:09 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * $? / $digit / quotes after $ / lone $: expand and advance *i. Else NULL.
 */
static char	*expand_special_var(t_shell *shell, size_t *i)
{
	char	c;

	c = shell->input[*i + 1];
	if (c == '?')
	{
		*i += 2;
		return (ft_itoa(shell->last_exit));
	}
	if (c == '"' || c == '\'' || ft_isdigit(c))
	{
		if (c == '"' || c == '\'')
			*i += 1;
		else
			*i += 2;
		return (ft_strdup(""));
	}
	if (!(ft_isalpha(c) || c == '_'))
	{
		(*i)++;
		return (ft_strdup("$"));
	}
	return (NULL);
}

/**
 * $NAME: lookup env, advance *i past name; caller frees returned string.
 */
static char	*expand_normal_var(t_shell *shell, size_t *i)
{
	size_t	start;
	size_t	len;
	char	*name;
	char	*value;

	start = *i + 1;
	len = 0;
	while (ft_isalnum(shell->input[start + len])
		|| shell->input[start + len] == '_')
		len++;
	name = ft_strndup(shell->input + start, len);
	if (!name)
	{
		*i = start + len;
		return (NULL);
	}
	value = get_env_value(shell->envp, name);
	free(name);
	*i = start + len;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

/**
 * Expand $ at *i: special forms first, then normal env var.
 */
char	*expand_var(t_shell *shell, size_t *i)
{
	char	*res;

	res = expand_special_var(shell, i);
	if (res)
		return (res);
	return (expand_normal_var(shell, i));
}

/**
 * Unquoted $: expand_var, append/split via append_expansion_unquoted. Returns 1.
 */
int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;
	size_t	start;

	if (shell->input[*i] != '$')
		return (0);
	start = *i;
	expanded = expand_var(shell, i);
	if (!expanded)
	{
		shell->last_exit = 1;
		return (1);
	}
	if (expanded[0] == '\0')
	{
		int	he;

		he = handle_empty_unquoted_expansion(shell, start, *i, word);
		if (he == MSH_OOM)
		{
			free(expanded);
			return (MSH_OOM);
		}
		if (he)
		{
			free(expanded);
			return (1);
		}
	}
	if (append_expansion_unquoted(shell, word, expanded, &shell->tokens)
		== MSH_OOM)
	{
		free(expanded);
		return (MSH_OOM);
	}
	free(expanded);
	return (1);
}

/** Leading ~ at word start: expand HOME (or empty) before / or boundary. */
int	handle_tilde_expansion(t_shell *shell, size_t *i, char **word)
{
	char	next;
	char	*home;

	if (shell->input[*i] != '~' || *word)
		return (0);
	next = shell->input[*i + 1];
	if (next && next != '/' && next != ' ' && next != '\t'
		&& !is_op_char(next))
		return (0);
	home = get_env_value(shell->envp, "HOME");
	if (!home)
		home = "";
	if (append_expansion_unquoted(shell, word, home, &shell->tokens)
		== MSH_OOM)
		return (MSH_OOM);
	(*i)++;
	return (1);
}

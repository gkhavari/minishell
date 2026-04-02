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
	len = msh_env_var_body_span(shell->input, start);
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

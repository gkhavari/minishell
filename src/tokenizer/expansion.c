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
 * Handle $? / $digit / quotes after $ / literal $ when not start of $NAME.
 * Advances *i; returns new string or NULL if caller should use exp_env.
 */
static char	*exp_spec(t_shell *shell, size_t *i)
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
 * Expand $NAME: copy env value (or ""), advance *i past the name.
 * Caller frees the returned string; returns NULL only on name alloc fail.
 */
static char	*exp_env(t_shell *shell, size_t *i)
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
 * Expand one $ token at *i (special forms first, else environment variable).
 * Advances *i; caller must free the returned string.
 */
char	*exp_var(t_shell *shell, size_t *i)
{
	char	*res;

	res = exp_spec(shell, i);
	if (res)
		return (res);
	return (exp_env(shell, i));
}

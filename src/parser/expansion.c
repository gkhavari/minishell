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

static char	*expand_special_var(t_shell *shell, size_t *i)
{
	size_t	start;
	char	buf[12];

	start = *i + 1;
	if (shell->input[start] == '?')
	{
		snprintf(buf, sizeof(buf), "%d", shell->last_exit);
		*i += 2;
		return (ft_strdup(buf));
	}
	if (!(ft_isalpha(shell->input[start]) || shell->input[start] == '_'))
	{
		(*i)++;
		return (ft_strdup("$"));
	}
	return (NULL);
}

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
	value = ft_strdup(get_env_value(shell->envp, name));
	free(name);
	*i = start + len;
	return (value);
}

char	*expand_var(t_shell *shell, size_t *i)
{
	char	*res;

	res = expand_special_var(shell, i);
	if (res)
		return (res);
	return (expand_normal_var(shell, i));
}

int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;

	if (shell->input[*i] != '$')
		return (0);
	expanded = expand_var(shell, i);
	append_expansion_unquoted(word, expanded, &shell->tokens);
	free(expanded);
	return (1);
}

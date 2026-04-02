/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/08 14:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_heredoc_named(char *line, size_t start, size_t *i,
		t_shell *shell)
{
	size_t	len;
	char	*name;
	char	*val;
	char	*out;

	len = msh_env_var_body_span(line, start);
	name = ft_strndup(line + start, len);
	if (name == NULL)
		return (NULL);
	val = get_env_value(shell->envp, name);
	free(name);
	*i = start + len;
	if (!val)
		return (ft_strdup(""));
	out = ft_strdup(val);
	return (out);
}

static char	*expand_heredoc_var(char *line, size_t *i, t_shell *shell)
{
	size_t	start;

	start = *i + 1;
	if (line[start] == '?')
	{
		*i += 2;
		return (ft_itoa(shell->last_exit));
	}
	return (expand_heredoc_named(line, start, i, shell));
}

static int	append_heredoc_char_or_var(char *line, size_t *i, char **result,
		t_shell *shell)
{
	char	*val;

	if (line[*i] == '$'
		&& msh_is_dollar_var_leader((unsigned char)line[*i + 1]))
	{
		val = expand_heredoc_var(line, i, shell);
		if (!val)
			return (-1);
		if (append_expansion_quoted(result, val) == MSH_OOM)
		{
			free(val);
			return (-1);
		}
		free(val);
	}
	else if (append_char(shell, result, line[(*i)++]) == MSH_OOM)
		return (-1);
	return (0);
}

/*
 * Expand $var and $? inside one heredoc line; NULL on OOM.
 * Caller still owns line.
 */
char	*expand_heredoc_line(char *line, t_shell *shell)
{
	char	*result;
	size_t	i;

	result = ft_strdup("");
	if (result == NULL)
		return (NULL);
	i = 0;
	while (line[i])
	{
		if (append_heredoc_char_or_var(line, &i, &result, shell) < 0)
		{
			free(result);
			return (NULL);
		}
	}
	return (result);
}

/** True if delim is wrapped in matching single or double quotes. */
int	is_quoted_delimiter(char *delim)
{
	size_t	len;

	if (!delim || !*delim)
		return (0);
	len = ft_strlen(delim);
	if (len < 2)
		return (0);
	if ((delim[0] == '\'' && delim[len - 1] == '\'')
		|| (delim[0] == '"' && delim[len - 1] == '"'))
		return (1);
	return (0);
}

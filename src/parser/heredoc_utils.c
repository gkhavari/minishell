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

static char	*expand_heredoc_var(char *line, size_t *i, t_shell *shell)
{
	size_t	start;
	size_t	len;
	char	*name;
	char	*val;

	start = *i + 1;
	if (line[start] == '?')
	{
		*i += 2;
		return (ft_itoa(shell->last_exit));
	}
	len = 0;
	while (ft_isalnum(line[start + len]) || line[start + len] == '_')
		len++;
	name = ft_strndup(line + start, len);
	val = get_env_value(shell->envp, name);
	free(name);
	*i = start + len;
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

char	*expand_heredoc_line(char *line, t_shell *shell)
{
	char	*result;
	char	*val;
	size_t	i;

	result = ft_strdup("");
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && (ft_isalpha(line[i + 1])
				|| line[i + 1] == '_' || line[i + 1] == '?'))
		{
			val = expand_heredoc_var(line, &i, shell);
			append_expansion_quoted(&result, val);
			free(val);
		}
		else
			append_char(shell, &result, line[i++]);
	}
	return (result);
}

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

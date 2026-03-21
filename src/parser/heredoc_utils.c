/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:26:19 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:41:42 by thanh-ng         ###   ########.fr       */
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

/**
 DESCRIPTION:
* Expand variables inside a heredoc line.

 BEHAVIOR:
* Walks the input `line`, replacing occurrences of `$NAME` or `$?` with
* their corresponding environment value or last exit code. Handles only
* variables allowed in heredoc context and appends characters to the
* result using shell-safe helpers.

 PARAMETERS:
* char *line: Input heredoc line to expand.
* t_shell *shell: Shell runtime providing `envp` and `last_exit`.

 RETURN:
* Newly allocated expanded string (caller must free).
*/
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

/**
 DESCRIPTION:
* Check whether a heredoc delimiter is quoted.

 BEHAVIOR:
* A delimiter is considered quoted when it begins and ends with matching
* single or double quotes and has length >= 2. Quoted delimiters suppress
* expansion in heredoc processing.

 PARAMETERS:
* char *delim: The delimiter string to inspect.

 RETURN:
* `1` if the delimiter is quoted, otherwise `0`.
*/


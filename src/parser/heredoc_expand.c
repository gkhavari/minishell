/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:21:15 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:21:17 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Expand $VAR name starting at start in line; advance *i past name.
 * Caller frees return; NULL on OOM.
 */
static char	*exp_hd_name(char *line, size_t start, size_t *i, t_shell *shell)
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

/**
 * Expand `$` at *i in a heredoc line (`?` or `NAME`); uses `exp_hd_name`.
 */
static char	*exp_hd_var(char *line, size_t *i, t_shell *shell)
{
	size_t	start;

	start = *i + 1;
	if (line[start] == '?')
	{
		*i += 2;
		return (ft_itoa(shell->last_exit));
	}
	return (exp_hd_name(line, start, i, shell));
}

/**
 * Append one char or one expanded $... from line at *i into *result.
 * Returns 0 on success, -1 on OOM or expansion failure.
 */
static int	hd_cat_step(char *line, size_t *i, char **result, t_shell *shell)
{
	char	*val;

	if (line[*i] == '$'
		&& msh_is_dollar_var_leader((unsigned char)line[*i + 1]))
	{
		val = exp_hd_var(line, i, shell);
		if (!val)
			return (-1);
		if (exp_q_cat(result, val) == OOM)
			return (free(val), -1);
		free(val);
	}
	else if (append_char(shell, result, line[(*i)++]) == OOM)
		return (-1);
	return (0);
}

/**
 * Build a new string with $VAR and $? expanded (unquoted heredoc body).
 * Caller frees return; NULL on OOM. Input line is not consumed.
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
		if (hd_cat_step(line, &i, &result, shell) < 0)
			return (free(result), NULL);
	}
	return (result);
}

/**
 * True if delimiter string is wrapped in matching '...' or "..." (no expand).
 */
int	is_quoted_delimiter(char *delim)
{
	size_t	len;

	if (!delim || !*delim)
		return (FALSE);
	len = ft_strlen(delim);
	if (len < 2)
		return (FALSE);
	if ((delim[0] == '\'' && delim[len - 1] == '\'')
		|| (delim[0] == '"' && delim[len - 1] == '"'))
		return (TRUE);
	return (FALSE);
}

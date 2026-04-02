/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_format.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	needs_dollar_quote(char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name && cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32
			|| (unsigned char)cmd_name[i] == MSH_ASCII_DEL)
			return (1);
		i++;
	}
	return (0);
}

static int	append_escaped_char(char *out, int j, char c)
{
	out[j++] = '\\';
	if (c == '\t')
		out[j++] = 't';
	else if (c == '\n')
		out[j++] = 'n';
	else if (c == '\r')
		out[j++] = 'r';
	else if (c == '\v')
		out[j++] = 'v';
	else if (c == '\f')
		out[j++] = 'f';
	else if (c == '\\')
		out[j++] = '\\';
	else if (c == '\'')
		out[j++] = '\'';
	else
		out[j++] = c;
	return (j);
}

static int	fill_dquote_body(char *out, int j, char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32
			|| (unsigned char)cmd_name[i] == MSH_ASCII_DEL
			|| cmd_name[i] == '\\' || cmd_name[i] == '\'')
			j = append_escaped_char(out, j, cmd_name[i]);
		else
			out[j++] = cmd_name[i];
		i++;
	}
	return (j);
}

/**
 * $'...' form when cmd_name has control bytes (exec error / not-found line).
 */
static char	*format_cmd_name_for_error(char *cmd_name)
{
	char	*out;
	int		j;

	if (!cmd_name || !needs_dollar_quote(cmd_name))
		return (NULL);
	out = malloc((ft_strlen(cmd_name) * 2) + 4);
	if (!out)
		return (NULL);
	out[0] = '$';
	out[1] = '\'';
	j = fill_dquote_body(out, 2, cmd_name);
	out[j++] = '\'';
	out[j] = '\0';
	return (out);
}

/** One line to stderr: optional $'…' quoting, then ": command not found". */
void	dprintf_cmd_not_found(char *cmd_name)
{
	char	*display;

	display = format_cmd_name_for_error(cmd_name);
	if (!display)
		display = cmd_name;
	ft_dprintf(STDERR_FILENO, "%s: command not found\n", display);
	if (display != cmd_name)
		free(display);
}

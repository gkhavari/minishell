/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_format.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 20:51:59 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	write_err_line(char *msg)
{
	if (!msg)
		return ;
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

/** Concatenate up to three strings to stderr (no alloc). */
void	write_err3(char *a, char *b, char *c)
{
	if (a)
		write_err_line(a);
	if (b)
		write_err_line(b);
	if (c)
		write_err_line(c);
}

static int	needs_dollar_quote(char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name && cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32
			|| (unsigned char)cmd_name[i] == 127)
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

/** $'...' style quoting for cmd not found when name has special bytes. */
char	*format_cmd_name_for_error(char *cmd_name)
{
	char	*out;
	int		i;
	int		j;

	if (!cmd_name || !needs_dollar_quote(cmd_name))
		return (NULL);
	out = malloc((ft_strlen(cmd_name) * 2) + 4);
	if (!out)
		return (NULL);
	out[0] = '$';
	out[1] = '\'';
	i = 0;
	j = 2;
	while (cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32 || (unsigned char)cmd_name[i] == 127
			|| cmd_name[i] == '\\' || cmd_name[i] == '\'')
			j = append_escaped_char(out, j, cmd_name[i]);
		else
			out[j++] = cmd_name[i];
		i++;
	}
	out[j++] = '\'';
	out[j] = '\0';
	return (out);
}

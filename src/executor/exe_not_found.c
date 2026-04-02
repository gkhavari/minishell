/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_not_found.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	need_dq(char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name && cmd_name[i])
	{
		if (!ft_isprint((unsigned char)cmd_name[i]))
			return (TRUE);
		i++;
	}
	return (FALSE);
}

static int	esc_c(char *out, int j, char c)
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

static int	fill_dq(char *out, int j, char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name[i])
	{
		if (!ft_isprint((unsigned char)cmd_name[i])
			|| cmd_name[i] == '\\' || cmd_name[i] == '\'')
			j = esc_c(out, j, cmd_name[i]);
		else
			out[j++] = cmd_name[i];
		i++;
	}
	return (j);
}

static char	*fmt_nf(char *cmd_name)
{
	char	*out;
	int		j;

	if (!cmd_name || !need_dq(cmd_name))
		return (NULL);
	out = malloc((ft_strlen(cmd_name) * 2) + 4);
	if (!out)
		return (NULL);
	out[0] = '$';
	out[1] = '\'';
	j = fill_dq(out, 2, cmd_name);
	out[j++] = '\'';
	out[j] = '\0';
	return (out);
}

/**
 * Print bash-style "cmd: command not found" to stderr; uses $'…' when the name
 * contains non-printable bytes.
 */
void	put_cmd_not_found(char *cmd_name)
{
	char	*display;

	display = fmt_nf(cmd_name);
	if (!display)
		display = cmd_name;
	ft_dprintf(STDERR_FILENO, "%s: command not found\n", display);
	if (display != cmd_name)
		free(display);
}

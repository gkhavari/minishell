/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_heredoc_line_stdin(t_shell *shell)
{
	char	*line;
	char	c;
	int		ret;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (!line || ft_strlen(line) == 0)
				return (free(line), NULL);
			return (line);
		}
		if (c == '\n')
			return (line);
		if (append_char(shell, &line, c) == MSH_OOM)
		{
			shell->oom = 1;
			return (NULL);
		}
	}
}

/*
** TTY: readline("> "); else byte-read until newline (cf. main read_line_stdin).
*/
char	*heredoc_read_line(t_shell *shell)
{
	if (isatty(STDIN_FILENO) == 1)
		return (readline("> "));
	return (read_heredoc_line_stdin(shell));
}

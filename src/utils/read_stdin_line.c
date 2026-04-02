/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_stdin_line.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Non-TTY: byte-read STDIN into *line until newline or EOF (no newline in buf).
 * Starts empty; grows via append_char (ft_realloc). READ_LINE / READ_EOF / OOM.
 * If set_shell_oom_on_fail and append fails, sets shell->oom.
 */
int	ft_read_stdin_line(t_shell *shell, char **line, int set_shell_oom_on_fail)
{
	char	c;
	int		ret;

	*line = ft_strdup("");
	if (!*line)
		return (OOM);
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (ft_strlen(*line) == 0)
				return (free(*line), *line = NULL, READ_EOF);
			return (READ_LINE);
		}
		if (c == '\n')
			return (READ_LINE);
		if (append_char(shell, line, c) == OOM)
		{
			if (set_shell_oom_on_fail)
				shell->oom = 1;
			return (OOM);
		}
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_stdin_read_line.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:22:58 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:23:01 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Read stdin byte-wise into *line until newline or EOF (grows via
 * `append_char`). Returns RL_LN, RL_EOF, or OOM; may set `shell->oom` on
 * append failure.
 */
int	ft_read_stdin_line(t_shell *shell, char **line, int set_shell_oom_on_fail)
{
	char	c;
	int		ret;

	*line = ft_strdup("");
	if (!*line)
		return (OOM);
	while (TRUE)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (ft_strlen(*line) == 0)
				return (free(*line), *line = NULL, RL_EOF);
			return (RL_LN);
		}
		if (c == '\n')
			return (RL_LN);
		if (append_char(shell, line, c) == OOM)
		{
			if (set_shell_oom_on_fail)
				shell->oom = 1;
			return (OOM);
		}
	}
}

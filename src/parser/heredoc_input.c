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

/*
** TTY: readline("> "); else ft_read_stdin_line (sets shell->oom on OOM).
*/
char	*heredoc_read_line(t_shell *shell)
{
	char	*line;
	int		st;

	if (isatty(STDIN_FILENO) == 1)
		return (readline("> "));
	st = ft_read_stdin_line(shell, &line, 1);
	if (st == READ_EOF || st == OOM)
		return (NULL);
	return (line);
}

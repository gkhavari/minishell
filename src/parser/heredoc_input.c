/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:38:09 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 18:45:07 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Bash-style EOF warning before wanted closing delimiter. */
void	print_heredoc_eof_warning(int line_no, char *delim)
{
	int	display_line;

	display_line = line_no;
	if (display_line < 1)
		display_line = 1;
	ft_dprintf(STDERR_FILENO,
		"warning: here-document at line %d delimited by end-of-file "
		"(wanted `%s')\n",
		display_line, delim);
}

/** Write line to heredoc pipe, optionally expanding variables. */
void	write_heredoc_line(char *line, int fd, int expand, t_shell *shell)
{
	char	*expanded;

	if (expand)
	{
		expanded = expand_heredoc_line(line, shell);
		if (!expanded)
		{
			shell->oom = 1;
			return ;
		}
		ft_dprintf(fd, "%s\n", expanded);
		free(expanded);
	}
	else
		ft_dprintf(fd, "%s\n", line);
}

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
	if (st == RL_EOF || st == OOM)
		return (NULL);
	return (line);
}

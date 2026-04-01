/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_warning.c                                  :+:      :+:    :+:   */
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
	ft_putstr_fd("warning: here-document at line ", 2);
	ft_putnbr_fd(display_line, 2);
	ft_putstr_fd(" delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(delim, 2);
	ft_putstr_fd("')\n", 2);
}

/** Write line to heredoc pipe, optionally expanding variables. */
void	write_heredoc_line(char *line, int fd, int expand, t_shell *shell)
{
	char	*expanded;

	if (expand)
	{
		expanded = expand_heredoc_line(line, shell);
		ft_putendl_fd(expanded, fd);
		free(expanded);
	}
	else
		ft_putendl_fd(line, fd);
}

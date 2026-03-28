/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:56:43 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/28 03:21:04 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_heredoc_eof_warning(int line_no, char *delim)
{
	int	display_line;

	display_line = line_no - 2;
	if (display_line < 2)
		display_line = 2;
	ft_putstr_fd("warning: here-document at line ", 2);
	ft_putnbr_fd(display_line, 2);
	ft_putstr_fd(" delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(delim, 2);
	ft_putstr_fd("')\n", 2);
}

static void	write_heredoc_line(char *line, int fd, int expand, t_shell *shell)
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

static char	*read_heredoc_line(t_shell *shell)
{
	char	*line;
	char	c;
	int		ret;

	if (isatty(STDIN_FILENO))
		return (readline("> "));
	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (ft_strlen(line) == 0)
				return (free(line), NULL);
			return (line);
		}
		if (c == '\n')
			return (line);
		append_char(shell, &line, c);
	}
}

static int	heredoc_read_loop(t_command *cmd, t_shell *shell, int *pipe_fd,
		int expand, int *line_no)
{
	char	*line;

	while (1)
	{
		line = read_heredoc_line(shell);
		if (g_signum == SIGINT)
			return (free(line), close(pipe_fd[0]), close(pipe_fd[1]), 1);
		if (!line)
		{
			print_heredoc_eof_warning(*line_no, cmd->heredoc_delim);
			break ;
		}
		(*line_no)++;
		if (ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(line, pipe_fd[1], expand, shell);
		free(line);
	}
	close(pipe_fd[1]);
	cmd->heredoc_fd = pipe_fd[0];
	return (0);
}

int	read_heredoc(t_command *cmd, t_shell *shell, int *line_no)
{
	int	pipe_fd[2];
	int	expand;

	if (pipe(pipe_fd) == -1)
		return (1);
	expand = !cmd->heredoc_quoted;
	return (heredoc_read_loop(cmd, shell, pipe_fd, expand, line_no));
}

int	process_heredocs(t_shell *shell)
{
	t_command	*cmd;
	int			line_no;

	cmd = shell->commands;
	line_no = 1;
	while (cmd)
	{
		if (cmd->heredoc_delim)
		{
			if (read_heredoc(cmd, shell, &line_no))
				return (1);
		}
		cmd = cmd->next;
	}
	return (0);
}

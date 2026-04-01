/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:56:43 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 17:37:57 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_heredoc_eof_warning(int line_no, char *delim);
void	write_heredoc_line(char *line, int fd, int expand, t_shell *shell);

static char	*read_heredoc_line(t_shell *shell)
{
	char	*line;
	char	c;
	int		ret;

	if (isatty(STDIN_FILENO) == 1)
		return (readline("> "));
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
		if (append_char(shell, &line, c) == FAILURE)
			return (NULL);
	}
}

static int	heredoc_interrupted(t_heredoc_ctx *ctx, char *line)
{
	free(line);
	close(ctx->pipe_fd[0]);
	close(ctx->pipe_fd[1]);
	return (FAILURE);
}

static int	heredoc_read_loop(t_heredoc_ctx *ctx, int *line_no,
		int start_line)
{
	char	*line;

	while (1)
	{
		line = read_heredoc_line(ctx->shell);
		if (g_signum == SIGINT)
			return (heredoc_interrupted(ctx, line));
		if (!line)
		{
			print_heredoc_eof_warning(start_line, ctx->cmd->heredoc_delim);
			break ;
		}
		(*line_no)++;
		if (ft_strcmp(line, ctx->cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(line, ctx->pipe_fd[1], ctx->expand, ctx->shell);
		free(line);
	}
	close(ctx->pipe_fd[1]);
	ctx->cmd->heredoc_fd = ctx->pipe_fd[0];
	return (SUCCESS);
}

int	read_heredoc(t_command *cmd, t_shell *shell, int *line_no)
{
	t_heredoc_ctx	ctx;
	int				start_line;

	if (pipe(ctx.pipe_fd) == -1)
		return (FAILURE);
	ctx.cmd = cmd;
	ctx.shell = shell;
	ctx.expand = !cmd->heredoc_quoted;
	start_line = *line_no + 1;
	return (heredoc_read_loop(&ctx, line_no, start_line));
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
				return (FAILURE);
		}
		cmd = cmd->next;
	}
	return (SUCCESS);
}

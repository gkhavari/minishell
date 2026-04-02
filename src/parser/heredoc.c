/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:56:43 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Close heredoc pipe ends; free optional partial line. FAILURE. */
static int	heredoc_interrupted(t_heredoc_ctx *ctx, char *line)
{
	free(line);
	close(ctx->pipe_fd[0]);
	close(ctx->pipe_fd[1]);
	return (FAILURE);
}

/**
 * Write line to pipe or match delimiter: HD_MORE, HD_DELIM, or -1 on OOM write.
 */
static int	heredoc_consume_line(t_heredoc_ctx *ctx, char *line, int *line_no)
{
	(*line_no)++;
	if (ft_strcmp(line, ctx->cmd->hd_delim) == 0)
		return (free(line), HD_DELIM);
	write_heredoc_line(line, ctx->pipe_fd[1], ctx->expand, ctx->shell);
	if (ctx->shell->oom)
		return (free(line), -1);
	return (free(line), HD_MORE);
}

/**
 * Read one heredoc line: -1 on interrupt/OOM; else HD_MORE, HD_DELIM, or HD_EOF.
 */
static int	heredoc_read_one(t_heredoc_ctx *ctx, int *line_no, int start_line)
{
	char	*line;
	int		st;

	line = heredoc_read_line(ctx->shell);
	if (g_signum == SIGINT || ctx->shell->oom)
	{
		if (ctx->shell->oom)
			ctx->shell->oom = 0;
		heredoc_interrupted(ctx, line);
		return (-1);
	}
	if (!line)
	{
		print_heredoc_eof_warning(start_line, ctx->cmd->hd_delim);
		return (HD_EOF);
	}
	st = heredoc_consume_line(ctx, line, line_no);
	if (st < 0)
	{
		heredoc_interrupted(ctx, NULL);
		return (-1);
	}
	return (st);
}

/** Open pipe, read lines until delimiter; heredoc read fd is `cmd->hd_fd`. */
int	read_heredoc(t_command *cmd, t_shell *shell, int *line_no)
{
	t_heredoc_ctx	ctx;
	int				start_line;
	int				r;

	if (pipe(ctx.pipe_fd) == -1)
		return (FAILURE);
	ctx.cmd = cmd;
	ctx.shell = shell;
	ctx.expand = !cmd->hd_quoted;
	start_line = *line_no + 1;
	while (TRUE)
	{
		r = heredoc_read_one(&ctx, line_no, start_line);
		if (r < 0)
			return (FAILURE);
		if (r > 0)
			break ;
	}
	close(ctx.pipe_fd[1]);
	cmd->hd_fd = ctx.pipe_fd[0];
	return (SUCCESS);
}

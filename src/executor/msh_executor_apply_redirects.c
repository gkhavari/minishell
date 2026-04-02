/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_executor_apply_redirects.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:01:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Open redirect file read-only, `dup2` onto standard input. */
static int	apply_input_redir(t_redir *r)
{
	int	fd;

	fd = open(r->file, O_RDONLY);
	if (fd == -1)
	{
		ft_dprintf(STDERR_FILENO, "%s: %s\n", r->file, strerror(errno));
		return (FAILURE);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (SUCCESS);
}

/** Open for write (truncate or append), `dup2` onto `r->fd`. */
static int	apply_output_redir(t_redir *r)
{
	int	fd;

	if (r->append)
		fd = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_dprintf(STDERR_FILENO, "%s: %s\n", r->file, strerror(errno));
		return (FAILURE);
	}
	dup2(fd, r->fd);
	close(fd);
	return (SUCCESS);
}

/** Reject ambiguous redirect token; dispatch `<` vs `>` / `>>`. */
static int	apply_one_redir(t_redir *r)
{
	size_t	prefix_len;

	prefix_len = ft_strlen(S_AMBIG);
	if (r->file && ft_strncmp(r->file, S_AMBIG, prefix_len) == 0)
	{
		ft_dprintf(STDERR_FILENO, "%s: ambiguous redirect\n",
			r->file + prefix_len);
		return (FAILURE);
	}
	if (r->fd == STDIN_FILENO)
		return (apply_input_redir(r));
	return (apply_output_redir(r));
}

/**
 * Apply `cmd->redirs` left-to-right (bash order). Then heredoc: dup to stdin
 * only if the last stdin redirect was `<<` (`stdin_last == STDIN_LAST_HD`);
 * always close the heredoc read fd.
 */
int	apply_redirs(t_command *cmd)
{
	t_list	*node;
	t_redir	*r;

	node = cmd->redirs;
	while (node)
	{
		r = node->content;
		if (apply_one_redir(r))
			return (FAILURE);
		node = node->next;
	}
	if (cmd->hd_fd != -1)
	{
		if (cmd->stdin_last == STDIN_LAST_HD)
			dup2(cmd->hd_fd, STDIN_FILENO);
		close(cmd->hd_fd);
		cmd->hd_fd = -1;
	}
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:01:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_input_redir(t_redir *r, int *had_input)
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
	*had_input = 1;
	return (SUCCESS);
}

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

static int	apply_one_redir(t_redir *r, int *had_input)
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
		return (apply_input_redir(r, had_input));
	return (apply_output_redir(r));
}

/**
 * Apply cmd->redirs (open + dup2). If heredoc_fd is set: dup to stdin only when
 * no '<' won stdin; always close heredoc_fd so the pipe read end is not leaked.
 */
int	apply_redirs(t_command *cmd)
{
	t_list	*node;
	t_redir	*r;
	int		had_input;

	had_input = 0;
	node = cmd->redirs;
	while (node)
	{
		r = node->content;
		if (apply_one_redir(r, &had_input))
			return (FAILURE);
		node = node->next;
	}
	if (cmd->heredoc_fd != -1)
	{
		if (!had_input)
			dup2(cmd->heredoc_fd, STDIN_FILENO);
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	return (SUCCESS);
}

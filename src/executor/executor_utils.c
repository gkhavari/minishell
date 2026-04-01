/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:01:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 23:59:14 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_redir_error(char *file, int err)
{
	char	*reason;
	char	*msg;
	size_t	file_len;
	size_t	reason_len;
	size_t	len;

	reason = strerror(err);
	file_len = ft_strlen(file);
	reason_len = ft_strlen(reason);
	len = file_len + 2 + reason_len + 1;
	msg = malloc(len + 1);
	if (!msg)
	{
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(reason, 2);
		ft_putstr_fd("\n", 2);
		return ;
	}
	ft_memcpy(msg, file, file_len);
	ft_memcpy(msg + file_len, ": ", 2);
	ft_memcpy(msg + file_len + 2, reason, reason_len);
	msg[len - 1] = '\n';
	write(2, msg, len);
	free(msg);
}

static int	apply_input_redir(t_redir *r, int *had_input)
{
	int	fd;

	fd = open(r->file, O_RDONLY);
	if (fd == -1)
	{
		print_redir_error(r->file, errno);
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
		print_redir_error(r->file, errno);
		return (FAILURE);
	}
	dup2(fd, r->fd);
	close(fd);
	return (SUCCESS);
}

/*
** apply_one_redir - Open one redirection and dup2 to stdin or stdout.
** Sets *had_input if it's an input redirect.
** Returns 0 on success, 1 on error.
*/
static int	apply_one_redir(t_redir *r, int *had_input)
{
	size_t	prefix_len;

	prefix_len = ft_strlen(MSH_AMBIG_REDIR_PREFIX);
	if (r->file && ft_strncmp(r->file, MSH_AMBIG_REDIR_PREFIX,
			prefix_len) == 0)
	{
		ft_putstr_fd(r->file + prefix_len, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		return (FAILURE);
	}
	if (r->fd == STDIN_FILENO)
		return (apply_input_redir(r, had_input));
	return (apply_output_redir(r));
}

/*
** apply_redirections - Apply all redirections in their original order.
** Processes cmd->redirs list in order, stopping on first failure.
** Applies heredoc only if no input file redirection exists.
*/
int	apply_redirections(t_command *cmd)
{
	t_redir	*r;
	int		had_input;

	had_input = 0;
	r = cmd->redirs;
	while (r)
	{
		if (apply_one_redir(r, &had_input))
			return (FAILURE);
		r = r->next;
	}
	if (cmd->heredoc_fd != -1 && !had_input)
	{
		dup2(cmd->heredoc_fd, STDIN_FILENO);
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	return (SUCCESS);
}

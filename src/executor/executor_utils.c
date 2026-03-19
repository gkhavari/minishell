/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/13 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** apply_one_redir - Open one redirection and dup2 to stdin or stdout.
** Sets *had_input if it's an input redirect.
** Returns 0 on success, 1 on error.
*/
static int	apply_one_redir(t_redir *r, int *had_input)
{
	int	fd;

	if (r->fd == STDIN_FILENO)
	{
		fd = open(r->file, O_RDONLY);
		if (fd == -1)
			return (perror("minishell"), 1);
		dup2(fd, STDIN_FILENO);
		close(fd);
		*had_input = 1;
	}
	else
	{
		if (r->append)
			fd = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
			return (perror("minishell"), 1);
		dup2(fd, r->fd);
		close(fd);
	}
	return (0);
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
			return (1);
		r = r->next;
	}
	if (cmd->heredoc_fd != -1 && !had_input)
	{
		dup2(cmd->heredoc_fd, STDIN_FILENO);
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	return (0);
}

void	restore_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

int	execute_builtin(t_command *cmd, t_shell *shell)
{
	return (run_builtin(cmd->argv, shell));
}

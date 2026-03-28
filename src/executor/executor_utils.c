/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:01:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/28 03:39:54 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_redir_error(char *file, int err)
{
	char	*tmp;
	char	*line;

	tmp = ft_strjoin(file, ": ");
	if (!tmp)
	{
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(err), 2);
		ft_putstr_fd("\n", 2);
		return ;
	}
	line = ft_strjoin(tmp, strerror(err));
	free(tmp);
	if (!line)
	{
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(err), 2);
		ft_putstr_fd("\n", 2);
		return ;
	}
	write(STDERR_FILENO, line, ft_strlen(line));
	write(STDERR_FILENO, "\n", 1);
	free(line);
}

/*
** apply_one_redir - Open one redirection and dup2 to stdin or stdout.
** Sets *had_input if it's an input redirect.
** Returns 0 on success, 1 on error.
*/
static int	apply_one_redir(t_redir *r, int *had_input)
{
	int	fd;
	size_t	prefix_len;

	prefix_len = ft_strlen(MSH_AMBIG_REDIR_PREFIX);
	if (r->file && ft_strncmp(r->file, MSH_AMBIG_REDIR_PREFIX,
			prefix_len) == 0)
	{
		ft_putstr_fd(r->file + prefix_len, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		return (1);
	}
	if (r->fd == STDIN_FILENO)
	{
		fd = open(r->file, O_RDONLY);
		if (fd == -1)
		{
			print_redir_error(r->file, errno);
			return (1);
		}
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
		{
			print_redir_error(r->file, errno);
			return (1);
		}
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

void	set_underscore(t_shell *shell, char *path)
{
	char	*entry;
	int		idx;

	if (!path)
		return ;
	entry = ft_strjoin("_=", path);
	if (!entry)
		return ;
	idx = find_export_key_index(shell, "_", 1);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
	}
	else
		(append_export_env(shell, entry), free(entry));
}

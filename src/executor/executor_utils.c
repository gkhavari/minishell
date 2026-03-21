/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:25:12 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:20:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Open and apply a single redirection for a command.

 BEHAVIOR:
* If the redirection targets stdin opens the file read-only, duplicates
* it to stdin and marks `*had_input`. Otherwise opens for writing with
* either append or truncate mode and duplicates to the specified fd.
* Returns non-zero on error.

 PARAMETERS:
* t_redir *r: Redirection descriptor containing `file`, `fd` and `append`.
* int *had_input: Output flag set when an input redirection was applied.

 RETURN:
* `0` on success, `1` on error.
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

/**
 DESCRIPTION:
* Apply all file redirections for a command in order.

 BEHAVIOR:
* Iterates `cmd->redirs` and applies each redirection via
* `apply_one_redir`. Tracks whether an input file redirection exists
* and only applies heredoc fallback when no input file was provided.

 PARAMETERS:
* t_command *cmd: Command whose redirections should be applied.

 RETURN:
* `0` on success, non-zero on failure.
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

/**
 DESCRIPTION:
* Restore previously backed-up stdin/stdout file descriptors.

 BEHAVIOR:
* Duplicates the backup descriptors back to `STDIN_FILENO` and
* `STDOUT_FILENO` then closes the backup descriptors.

 PARAMETERS:
* int stdin_backup: Backup descriptor for stdin.
* int stdout_backup: Backup descriptor for stdout.
*/
void	restore_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

/**
 DESCRIPTION:
* Execute a builtin command in the parent process.

 BEHAVIOR:
* Delegates to `run_builtin` forwarding the command argv and shell state.

 PARAMETERS:
* t_command *cmd: Command node containing `argv`.
* t_shell *shell: Shell runtime state.

 RETURN:
* Exit/status code returned by the builtin.
*/
int	execute_builtin(t_command *cmd, t_shell *shell)
{
	return (run_builtin(cmd->argv, shell));
}

/**
 DESCRIPTION:
* Update the special `_` environment variable to point to `path`.

 BEHAVIOR:
* Constructs an `_=` entry and replaces an existing `_` variable in
* `shell->envp` or appends the entry if not present.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `envp` is modified.
* char *path: Path to set as the value of `_`.
*/
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

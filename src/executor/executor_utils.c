/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:01:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 21:43:51 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Print a redirection-related error message to STDERR.

 BEHAVIOR:
* Builds a "file: strerror(err)\n" message and writes it to
* `STDERR_FILENO`. Attempts an allocated buffer first and falls
* back to multiple writes on allocation failure.

 PARAMETERS:
* char *file: filename associated with the error.
* int err: errno value describing the error.

 RETURN:
* None.
*/
static void	print_redir_error(char *file, int err)
{
	char	*line;
	char	*msg;
	size_t	len;

	msg = strerror(err);
	len = ft_strlen(file) + ft_strlen(msg) + 4;
	line = malloc(len);
	if (!line)
	{
		ft_putstr_fd(file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(msg, 2);
		ft_putstr_fd("\n", 2);
		return ;
	}
	ft_strlcpy(line, file, len);
	ft_strlcat(line, ": ", len);
	ft_strlcat(line, msg, len);
	ft_strlcat(line, "\n", len);
	write(STDERR_FILENO, line, ft_strlen(line));
	free(line);
}

/**
 DESCRIPTION:
* Apply an input redirection by opening the target file and dup2-ing
* it onto `STDIN_FILENO`.

 BEHAVIOR:
* Opens the file read-only, on error prints a diagnostics and returns
* non-zero. On success duplicates the fd to stdin, closes the
* original and marks `*had_input`.

 PARAMETERS:
* t_redir *r: redirection descriptor.
* int *had_input: out-parameter set to 1 if an input redirection was
*   successfully applied.

 RETURN:
* 0 on success, non-zero on error.
*/
static int	apply_input_redir(t_redir *r, int *had_input)
{
	int	fd;

	fd = open(r->file, O_RDONLY);
	if (fd == -1)
	{
		print_redir_error(r->file, errno);
		return (1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	*had_input = 1;
	return (0);
}

/**
 DESCRIPTION:
* Apply an output redirection by opening/truncating/creating the file
* and dup2-ing it onto the requested fd.

 BEHAVIOR:
* Honors append vs truncate flags, reports errors via
* `print_redir_error` and returns non-zero on failure.

 PARAMETERS:
* t_redir *r: redirection descriptor.

 RETURN:
* 0 on success, non-zero on error.
*/
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
		return (1);
	}
	dup2(fd, r->fd);
	close(fd);
	return (0);
}

/**
 DESCRIPTION:
* Apply a single redirection entry from the command's redirection list.

 BEHAVIOR:
* Detects ambiguous heredoc-placeholder filenames and prints an
* appropriate message. Dispatches to input or output helper for the
* actual file operations.

 PARAMETERS:
* t_redir *r: redirection entry to apply.
* int *had_input: pointer indicating if an input redirection was
*   applied earlier in the list.

 RETURN:
* 0 on success, non-zero on error.
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
		return (1);
	}
	if (r->fd == STDIN_FILENO)
		return (apply_input_redir(r, had_input));
	return (apply_output_redir(r));
}

/**
 DESCRIPTION:
* Apply all redirections for the given command in the original order.

 BEHAVIOR:
* Iterates `cmd->redirs` and applies each entry. If an input file
* redirection is applied, any pending heredoc is ignored. Stops on
* first error and returns non-zero.

 PARAMETERS:
* t_command *cmd: command whose redirections should be applied.

 RETURN:
* 0 on success, non-zero on the first redirection error.
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
		{
			if (cmd->heredoc_fd != -1)
				close(cmd->heredoc_fd);
			cmd->heredoc_fd = -1;
			return (1);
		}
		r = r->next;
	}
	if (cmd->heredoc_fd == -1)
		return (0);
	if (!had_input)
		dup2(cmd->heredoc_fd, STDIN_FILENO);
	close(cmd->heredoc_fd);
	cmd->heredoc_fd = -1;
	return (0);
}

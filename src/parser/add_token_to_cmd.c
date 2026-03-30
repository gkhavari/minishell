/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:19:21 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Store the heredoc delimiter from the token following a HEREDOC token.

 BEHAVIOR:
* Frees any previously stored delimiter (supporting multiple heredocs)
* and duplicates the following token's value into `cmd->heredoc_delim`.
* The actual heredoc content is read later in `process_heredocs`.

 PARAMETERS:
* t_command *cmd: Command node to store the delimiter in.
* t_token *token: Token node pointing at the HEREDOC token.

 RETURN:
* None.
*/
static void	handle_heredoc_token(t_command *cmd, t_token *token)
{
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = ft_strdup(token->next->value);
	cmd->heredoc_quoted = token->next->quoted;
}

/**
 DESCRIPTION:
* Append a redirection node to a command's redirection list.

 BEHAVIOR:
* Allocates a `t_redir` node with the given `file`, `fd` and `append`
* flag and appends it to `cmd->redirs` preserving order.

 PARAMETERS:
* t_command *cmd: Command to receive the redirection.
* char *file: Filename for the redirection.
* int fd: Target file descriptor (e.g., STDIN_FILENO, STDOUT_FILENO).
* int append: Non-zero for append mode (>>), zero for truncate (>).

 RETURN:
* None.
*/
static void	append_redir(t_command *cmd, char *file, int fd, int append)
{
	t_redir	*r;
	t_redir	*tmp;

	r = malloc(sizeof(t_redir));
	if (!r)
		return ;
	r->file = ft_strdup(file);
	r->fd = fd;
	r->append = append;
	r->next = NULL;
	if (!cmd->redirs)
		cmd->redirs = r;
	else
	{
		tmp = cmd->redirs;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = r;
	}
}

/**
 DESCRIPTION:
* Append a `word` as an argument node to `cmd->args`.

 BEHAVIOR:
* Allocates a `t_arg` node, duplicates `word` into it and appends it
* to the end of the command's argument linked list.

 PARAMETERS:
* t_shell *shell: Shell runtime used for allocator `msh_calloc`.
* t_command *cmd: Command node to receive the argument.
* char *word: Argument string to append.

 RETURN:
* None.
*/
static void	add_word_to_cmd(t_shell *shell, t_command *cmd,
	char *word, int quoted)
{
	t_arg	*new;
	t_arg	*tmp;

	if (!word || (!*word && !quoted))
		return ;
	new = msh_calloc(shell, 1, sizeof(t_arg));
	new->value = ft_strdup(word);
	new->next = NULL;
	if (!cmd->args)
		cmd->args = new;
	else
	{
		tmp = cmd->args;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

/**
 DESCRIPTION:
* Dispatch a single token into the command structure, creating args or
* redirection nodes as appropriate.

 BEHAVIOR:
* - WORD tokens are appended as argument nodes.
* - HEREDOC consumes the following token as a delimiter and stores it.
* - Redirection tokens allocate and append `t_redir` nodes.
* The function performs defensive checks and will return `FAILURE` on
* allocation errors to signal the caller to abort command construction.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation helpers.
* t_command *cmd: Command node to modify.
* t_token *token: Token to process.

 RETURN:
* `1` when a WORD was added, `2` when a redirection/heredoc was handled,
* or `FAILURE` on error (allocation or malformed token sequence).
*/
int	add_token_to_command(t_shell *shell, t_command *cmd, t_token *token)
{
	if (token->type == WORD)
	{
		add_word_to_cmd(shell, cmd, token->value, token->quoted);
		return (1);
	}
	if (token->type == HEREDOC)
	{
		handle_heredoc_token(cmd, token);
		return (2);
	}
	if (token->type == REDIR_IN)
		append_redir(cmd, token->next->value, STDIN_FILENO, 0);
	else if (token->type == REDIR_OUT)
		append_redir(cmd, token->next->value, STDOUT_FILENO, 0);
	else if (token->type == APPEND)
		append_redir(cmd, token->next->value, STDOUT_FILENO, 1);
	else if (token->type == REDIR_ERR_OUT)
		append_redir(cmd, token->next->value, STDERR_FILENO, 0);
	else
		return (1);
	return (2);
}

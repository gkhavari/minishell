/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 17:26:08 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** handle_heredoc_token - Store heredoc delimiter from the next token
** Frees any previous delimiter (handles multiple heredocs per command).
** The actual heredoc reading happens later in process_heredocs().
*/
static void	handle_heredoc_token(t_command *cmd, t_token *token)
{
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = ft_strdup(token->next->value);
	cmd->heredoc_quoted = token->next->quoted;
}

/*
** append_redir - Append a redirection node to cmd->redirs (ordered list).
** is_input=1 for <, is_input=0 for > or >>. append=1 for >>.
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
 * Appends a new argument to the command's argument list.
 */
static void	add_word_to_cmd(t_shell *shell, t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

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

/*
** add_token_to_command - Dispatch a token into the command structure
** WORD tokens become command arguments.
** Redirection tokens (< > >> <<) set the appropriate file/delimiter.
** Note: parse_tokens() already skips the filename WORD after redirections.
** Returns: 1 for WORD, 2 for redir/heredoc, FAILURE on error.
*/
int	add_token_to_command(t_shell *shell, t_command *cmd, t_token *token)
{
	if (token->type == WORD)
	{
		add_word_to_cmd(shell, cmd, token->value);
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

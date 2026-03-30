/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_empty_expand_token(char *value)
{
	if (!value)
		return (0);
	return (ft_strcmp(value, MSH_EMPTY_EXPAND_TOKEN) == 0);
}

/*
** handle_heredoc_token - Store heredoc delimiter from the next token
** Frees any previous delimiter (handles multiple heredocs per command).
** The actual heredoc reading happens later in process_heredocs().
*/
static int	handle_heredoc_token(t_command *cmd, t_token *token)
{
	char	*new_delim;

	if (!token->next || !token->next->value)
		return (FAILURE);
	new_delim = ft_strdup(token->next->value);
	if (!new_delim)
		return (FAILURE);
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = new_delim;
	cmd->heredoc_quoted = token->next->quoted;
	return (SUCCESS);
}

/*
** append_redir - Append a redirection node to cmd->redirs (ordered list).
** is_input=1 for <, is_input=0 for > or >>. append=1 for >>.
*/
static int	append_redir(t_command *cmd, char *file, int fd, int append)
{
	t_redir	*r;
	t_redir	*tmp;

	r = malloc(sizeof(t_redir));
	if (!r)
		return (FAILURE);
	r->file = ft_strdup(file);
	if (!r->file)
	{
		free(r);
		return (FAILURE);
	}
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
	return (SUCCESS);
}

/**
 * Appends a new argument to the command's argument list.
 */
static int	add_word_to_cmd(t_shell *shell, t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

	(void)shell;
	new = malloc(sizeof(t_arg));
	if (!new)
		return (FAILURE);
	new->value = ft_strdup(word);
	if (!new->value)
	{
		free(new);
		return (FAILURE);
	}
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
	return (SUCCESS);
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
		if (is_empty_expand_token(token->value))
			return (1);
		if (add_word_to_cmd(shell, cmd, token->value) == FAILURE)
			return (FAILURE);
		return (1);
	}
	if (token->type == HEREDOC)
	{
		if (handle_heredoc_token(cmd, token) == FAILURE)
			return (FAILURE);
		return (2);
	}
	if (!token->next || !token->next->value)
		return (FAILURE);
	if (token->type == REDIR_IN)
	{
		if (append_redir(cmd, token->next->value, STDIN_FILENO, 0) == FAILURE)
			return (FAILURE);
		return (2);
	}
	else if (token->type == REDIR_OUT)
	{
		if (append_redir(cmd, token->next->value, STDOUT_FILENO, 0) == FAILURE)
			return (FAILURE);
		return (2);
	}
	else if (token->type == APPEND)
	{
		if (append_redir(cmd, token->next->value, STDOUT_FILENO, 1) == FAILURE)
			return (FAILURE);
		return (2);
	}
	else
		return (1);
}

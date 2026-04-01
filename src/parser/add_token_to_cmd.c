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

/** Next WORD is heredoc delimiter; replaces any previous delim. */
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

/** Append one t_redir to cmd->redirs. */
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

static int	add_redir_token(t_command *cmd, t_token *token)
{
	if (!token->next || !token->next->value)
		return (PARSE_ERR);
	if (token->type == REDIR_IN)
	{
		if (append_redir(cmd, token->next->value, STDIN_FILENO, 0) == FAILURE)
			return (PARSE_ERR);
		return (2);
	}
	if (token->type == REDIR_OUT)
	{
		if (append_redir(cmd, token->next->value, STDOUT_FILENO, 0) == FAILURE)
			return (PARSE_ERR);
		return (2);
	}
	if (token->type == APPEND)
	{
		if (append_redir(cmd, token->next->value, STDOUT_FILENO, 1) == FAILURE)
			return (PARSE_ERR);
		return (2);
	}
	return (1);
}

/**
 * WORD → args; redir/heredoc → file or delim. Returns 1, 2, or PARSE_ERR.
 */
int	add_token_to_command(t_shell *shell, t_command *cmd, t_token *token)
{
	if (token->type == WORD)
	{
		if (token->value
			&& ft_strcmp(token->value, MSH_EMPTY_EXPAND_TOKEN) == 0)
			return (1);
		if (add_word_to_cmd(shell, cmd, token->value) == FAILURE)
			return (PARSE_ERR);
		return (1);
	}
	if (token->type == HEREDOC)
	{
		if (handle_heredoc_token(cmd, token) == FAILURE)
			return (PARSE_ERR);
		return (2);
	}
	return (add_redir_token(cmd, token));
}

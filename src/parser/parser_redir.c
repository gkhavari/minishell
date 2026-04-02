/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	append_redir(t_command *cmd, char *file, int fd, int append)
{
	t_redir	*r;
	t_list	*node;

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
	node = ft_lstnew(r);
	if (!node)
	{
		free(r->file);
		free(r);
		return (FAILURE);
	}
	ft_lstadd_back(&cmd->redirs, node);
	return (SUCCESS);
}

static int	do_redir_in(t_command *cmd, char *file)
{
	if (append_redir(cmd, file, STDIN_FILENO, 0) == FAILURE)
		return (PARSE_ERR);
	return (2);
}

static int	do_redir_out(t_command *cmd, char *file)
{
	if (append_redir(cmd, file, STDOUT_FILENO, 0) == FAILURE)
		return (PARSE_ERR);
	return (2);
}

static int	do_redir_append(t_command *cmd, char *file)
{
	if (append_redir(cmd, file, STDOUT_FILENO, 1) == FAILURE)
		return (PARSE_ERR);
	return (2);
}

/**
 * Redirection token + following WORD. Returns 2 or PARSE_ERR (or 1 fallback).
 */
int	parse_redir_token_pair(t_command *cmd, t_list *tok_node)
{
	t_token	*next_tok;
	t_token	*cur;

	cur = tok_node->content;
	if (!tok_node->next)
		return (PARSE_ERR);
	next_tok = tok_node->next->content;
	if (!next_tok->value)
		return (PARSE_ERR);
	if (cur->type == REDIR_IN)
		return (do_redir_in(cmd, next_tok->value));
	if (cur->type == REDIR_OUT)
		return (do_redir_out(cmd, next_tok->value));
	if (cur->type == APPEND)
		return (do_redir_append(cmd, next_tok->value));
	return (1);
}

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

/** Push `t_redir` onto `cmd->redirs`; updates `stdin_last` for `<`. */
static int	append_redir(t_command *cmd, char *file, int fd, int append)
{
	t_redir	*r;
	t_list	*node;

	r = malloc(sizeof(t_redir));
	if (!r)
		return (OOM);
	r->file = ft_strdup(file);
	if (!r->file)
		return (free(r), OOM);
	r->fd = fd;
	r->append = append;
	if (fd == STDIN_FILENO)
		cmd->stdin_last = STDIN_LAST_FILE;
	node = ft_lstnew(r);
	if (!node)
		return (free(r->file), free(r), OOM);
	ft_lstadd_back(&cmd->redirs, node);
	return (SUCCESS);
}

/** Call `append_redir`; returns PR_2 or OOM when `append_redir` fails. */
static int	redir_pair(t_command *cmd, char *file, int fd, int append)
{
	if (append_redir(cmd, file, fd, append) == OOM)
		return (OOM);
	return (PR_2);
}

/**
 * Redirection token + following WORD.
 * Returns PR_2, PR_1, PR_ERR, or OOM.
 */
int	parse_redir_token_pair(t_command *cmd, t_list *tok_node)
{
	t_token	*next_tok;
	t_token	*cur;

	cur = tok_node->content;
	if (!tok_node->next)
		return (PR_ERR);
	next_tok = tok_node->next->content;
	if (!next_tok->value)
		return (PR_ERR);
	if (cur->type == REDIR_IN)
		return (redir_pair(cmd, next_tok->value, STDIN_FILENO, 0));
	if (cur->type == REDIR_OUT)
		return (redir_pair(cmd, next_tok->value, STDOUT_FILENO, 0));
	if (cur->type == APPEND)
		return (redir_pair(cmd, next_tok->value, STDOUT_FILENO, 1));
	return (PR_1);
}

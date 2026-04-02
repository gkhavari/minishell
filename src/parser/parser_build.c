/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_build.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	append_pipe_command(t_shell *shell, t_command **cmd,
		t_list **tok_node, t_list **cmds_root)
{
	t_command	*nc;
	t_list		*node;

	nc = ft_calloc(1, sizeof(t_command));
	if (!nc)
	{
		shell->last_exit = FAILURE;
		free_commands(cmds_root);
		return (OOM);
	}
	nc->heredoc_fd = -1;
	node = ft_lstnew(nc);
	if (!node)
	{
		free(nc);
		free_commands(cmds_root);
		return (OOM);
	}
	ft_lstadd_back(cmds_root, node);
	*cmd = nc;
	*tok_node = (*tok_node)->next;
	return (SUCCESS);
}

static int	parse_token_nonpipe(t_shell *shell, t_command *cmd,
		t_list **tok_node, t_list **cmds_root)
{
	int	consumed;

	consumed = add_token_to_command(shell, cmd, *tok_node);
	if (consumed == OOM)
	{
		free_commands(cmds_root);
		shell->oom = 1;
		return (OOM);
	}
	if (consumed == PR_ERR)
	{
		free_commands(cmds_root);
		return (FAILURE);
	}
	while (consumed > 0 && *tok_node)
	{
		*tok_node = (*tok_node)->next;
		consumed--;
	}
	return (SUCCESS);
}

/**
 * One parse step: pipe splits command; else add_token_to_command and advance.
 */
static int	parse_token_step(t_shell *shell, t_command **cmd,
		t_list **tok_node, t_list **cmds_root)
{
	t_token	*t;
	int		p;

	t = (*tok_node)->content;
	if (t->type == PIPE)
	{
		p = append_pipe_command(shell, cmd, tok_node, cmds_root);
		if (p == OOM)
		{
			shell->oom = 1;
			return (OOM);
		}
		return (SUCCESS);
	}
	return (parse_token_nonpipe(shell, *cmd, tok_node, cmds_root));
}

static t_list	*cmdlist_first_node(t_shell *shell, t_command **cmd_out)
{
	t_command	*c;
	t_list		*lst;

	c = ft_calloc(1, sizeof(t_command));
	if (!c)
	{
		shell->last_exit = FAILURE;
		shell->oom = 1;
		return (NULL);
	}
	c->heredoc_fd = -1;
	lst = ft_lstnew(c);
	if (!lst)
	{
		shell->oom = 1;
		free(c);
		return (NULL);
	}
	*cmd_out = c;
	return (lst);
}

/**
 * Walk token list into a pipeline of t_command (t_list nodes).
 */
t_list	*build_command_list(t_shell *shell, t_list *tokens)
{
	t_command	*cmd;
	t_list		*cmds;
	t_list		*tok;
	int			st;

	cmds = cmdlist_first_node(shell, &cmd);
	if (!cmds)
		return (NULL);
	tok = tokens;
	while (tok)
	{
		st = parse_token_step(shell, &cmd, &tok, &cmds);
		if (st == OOM || st == FAILURE)
			return (NULL);
	}
	return (cmds);
}

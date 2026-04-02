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

/**
 * calloc(1) t_command; heredoc_fd = -1; NULL on failure.
 */
static t_command	*new_command(t_shell *shell)
{
	t_command	*cmd;

	cmd = ft_calloc(1, sizeof(t_command));
	if (!cmd)
	{
		shell->last_exit = FAILURE;
		return (NULL);
	}
	if (cmd)
		cmd->heredoc_fd = -1;
	return (cmd);
}

static int	append_pipe_command(t_shell *shell, t_command **cmd,
		t_list **tok_node, t_list **cmds_root)
{
	t_command	*nc;
	t_list		*node;

	nc = new_command(shell);
	if (!nc)
	{
		free_commands(cmds_root);
		return (FAILURE);
	}
	node = ft_lstnew(nc);
	if (!node)
	{
		free(nc);
		free_commands(cmds_root);
		return (FAILURE);
	}
	ft_lstadd_back(cmds_root, node);
	*cmd = nc;
	*tok_node = (*tok_node)->next;
	return (SUCCESS);
}

/**
 * One parse step: pipe splits command; else add_token_to_command and advance.
 */
static int	parse_token_step(t_shell *shell, t_command **cmd,
		t_list **tok_node, t_list **cmds_root)
{
	int			consumed;
	t_token		*t;

	t = (*tok_node)->content;
	if (t->type == PIPE)
		return (append_pipe_command(shell, cmd, tok_node, cmds_root));
	consumed = add_token_to_command(shell, *cmd, *tok_node);
	if (consumed == PARSE_ERR)
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
 * Walk token list into a pipeline of t_command (t_list nodes).
 */
t_list	*build_command_list(t_shell *shell, t_list *tokens)
{
	t_command	*first_cmd;
	t_list		*cmds;
	t_command	*cmd;
	t_list		*tok;

	first_cmd = new_command(shell);
	if (!first_cmd)
		return (NULL);
	cmds = ft_lstnew(first_cmd);
	if (!cmds)
		return (free(first_cmd), NULL);
	cmd = first_cmd;
	tok = tokens;
	while (tok)
	{
		if (parse_token_step(shell, &cmd, &tok, &cmds) == FAILURE)
			return (NULL);
	}
	return (cmds);
}

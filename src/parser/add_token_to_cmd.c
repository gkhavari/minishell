/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Next WORD is the heredoc delimiter string; replaces any previous one. */
static int	handle_heredoc_token(t_command *cmd, t_list *tok_node)
{
	char		*new_delim;
	t_token		*next_tok;

	if (!tok_node->next)
		return (PR_ERR);
	next_tok = tok_node->next->content;
	if (!next_tok->value)
		return (PR_ERR);
	new_delim = ft_strdup(next_tok->value);
	if (!new_delim)
		return (OOM);
	if (cmd->hd_delim)
		free(cmd->hd_delim);
	cmd->hd_delim = new_delim;
	cmd->hd_quoted = next_tok->quoted;
	cmd->stdin_last = STDIN_LAST_HD;
	return (SUCCESS);
}

/**
 * Appends a new argument to the command's argument list.
 */
static int	add_word_to_cmd(t_shell *shell, t_command *cmd, char *word)
{
	t_arg	*new;
	t_list	*node;

	(void)shell;
	new = malloc(sizeof(t_arg));
	if (!new)
		return (FAILURE);
	new->value = ft_strdup(word);
	if (!new->value)
		return (free(new), FAILURE);
	node = ft_lstnew(new);
	if (!node)
		return (free(new->value), free(new), FAILURE);
	ft_lstadd_back(&cmd->args, node);
	return (SUCCESS);
}

/**
 * WORD adds an argument; redirects and heredoc consume file or delimiter.
 * Returns advance count, PR_ERR for structural errors, or OOM.
 */
int	add_token_to_command(t_shell *shell, t_command *cmd, t_list *tok_node)
{
	t_token	*token;
	int		hr;

	token = tok_node->content;
	if (token->type == WORD)
	{
		if (token->value
			&& ft_strcmp(token->value, S_EMPTY) == 0)
			return (PR_1);
		if (add_word_to_cmd(shell, cmd, token->value) == FAILURE)
			return (OOM);
		return (PR_1);
	}
	if (token->type == HEREDOC)
	{
		hr = handle_heredoc_token(cmd, tok_node);
		if (hr == OOM)
			return (OOM);
		if (hr != SUCCESS)
			return (PR_ERR);
		return (PR_2);
	}
	return (parse_redir_token_pair(cmd, tok_node));
}

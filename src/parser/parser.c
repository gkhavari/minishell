/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
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

/**
 * One parse step: pipe splits command; else add_token_to_command and advance.
 */
static int	parse_token_step(t_shell *shell, t_command **cmd,
		t_token **token, t_command *head)
{
	int	consumed;

	if ((*token)->type == PIPE)
	{
		(*cmd)->next = new_command(shell);
		if (!(*cmd)->next)
			return (free_commands(head), FAILURE);
		*cmd = (*cmd)->next;
		*token = (*token)->next;
		return (SUCCESS);
	}
	consumed = add_token_to_command(shell, *cmd, *token);
	if (consumed == PARSE_ERR)
		return (free_commands(head), FAILURE);
	while (consumed > 0 && *token)
	{
		*token = (*token)->next;
		consumed--;
	}
	return (SUCCESS);
}

static t_command	*parse_tokens(t_shell *shell, t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command(shell);
	if (!head)
		return (NULL);
	cmd = head;
	while (token)
	{
		if (parse_token_step(shell, &cmd, &token, head) == FAILURE)
			return (NULL);
	}
	return (head);
}

/**
 * syntax_check → parse_tokens → finalize_all_commands; frees token list.
 */
void	parse_input(t_shell *shell)
{
	if (!shell->tokens)
	{
		shell->commands = NULL;
		return ;
	}
	if (syntax_check(shell->tokens) == SYNTAX_ERR)
	{
		shell->last_exit = EXIT_SYNTAX_ERROR;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	shell->commands = parse_tokens(shell, shell->tokens);
	free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (!shell->commands)
	{
		shell->last_exit = FAILURE;
		return ;
	}
	finalize_all_commands(shell, shell->commands);
}

/** True if type is <, >, >>, or <<. */
int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT || type == APPEND
		|| type == HEREDOC);
}

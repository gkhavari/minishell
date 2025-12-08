/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:01:16 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_token_to_command(t_command *cmd, t_token *token)
{
	if (token->type == WORD)
		add_word_to_cmd(cmd, token->value);
	else if (token->type == REDIR_IN)
		cmd->input_file = ft_strdup(token->next->value);
	else if (token->type == REDIR_OUT)
	{
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 0;
	}
	else if (token->type == APPEND)
	{
		cmd->output_file = ft_strdup(token->next->value);
		cmd->append = 1;
	}
	else if (token->type == HEREDOC)
		process_heredoc(cmd, token->next->value);
}

void	add_word_to_cmd(t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

	new = malloc(sizeof(t_arg));
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

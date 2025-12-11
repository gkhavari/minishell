/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/05 22:42:06 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->argv = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = 0;
	cmd->is_builtin = 0;
	cmd->next = NULL;
	return (cmd);
}

t_command	*parse_tokens(t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command();
	cmd = head;
	while (token)
	{
		if (token->type == PIPE)
		{
			cmd->next = new_command();
			cmd = cmd->next;
		}
		else
			add_token_to_command(cmd, token);
		token = token->next;
	}
	return (head);
}

void	parse_input(t_shell *shell)
{
	if (syntax_check(shell->tokens))
		return ;
	shell->commands = parse_tokens(shell->tokens);
	finalize_all_commands(shell->commands);
}

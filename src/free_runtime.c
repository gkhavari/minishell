/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/02/24 22:20:46 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(t_token *token)
{
	t_token	*tmp;

	while (token)
	{
		tmp = token->next;
		if (token->value)
			free(token->value);
		free(token);
		token = tmp;
	}
}

void	free_args(t_arg *arg)
{
	t_arg	*tmp;

	while (arg)
	{
		tmp = arg->next;
		if (arg->value)
			free(arg->value);
		free(arg);
		arg = tmp;
	}
}

static void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

void	free_commands(t_command *cmd)
{
	t_command	*tmp;

	while (cmd)
	{
		if (cmd->input_file && is_heredoc(cmd->input_file))
			unlink(cmd->input_file);
		if (cmd->heredoc_fd != -1)
			close(cmd->heredoc_fd);
		tmp = cmd->next;
		free_args(cmd->args);
		free_argv(cmd->argv);
		if (cmd->input_file)
			free(cmd->input_file);
		if (cmd->output_file)
			free(cmd->output_file);
		if (cmd->heredoc_delim)
			free(cmd->heredoc_delim);
		free(cmd);
		cmd = tmp;
	}
}

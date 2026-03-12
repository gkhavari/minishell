/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static void	free_out_redirs(t_redir *r)
{
	t_redir	*tmp;

	while (r)
	{
		tmp = r->next;
		free(r->file);
		free(r);
		r = tmp;
	}
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
		free_out_redirs(cmd->out_redirs);
		if (cmd->output_file)
			free(cmd->output_file);
		if (cmd->heredoc_delim)
			free(cmd->heredoc_delim);
		free(cmd);
		cmd = tmp;
	}
}

static void	free_envp(char **envp)
{
	int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

void	free_all(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	if (shell->envp)
		free_envp(shell->envp);
	free(shell->user);
	free(shell->cwd);
	free(shell->input);
}

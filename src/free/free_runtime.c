/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 17:25:49 by thanh-ng         ###   ########.fr       */
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
		if (cmd->heredoc_fd != -1)
			close(cmd->heredoc_fd);
		tmp = cmd->next;
		free_args(cmd->args);
		free_argv(cmd->argv);
		free_out_redirs(cmd->redirs);
		if (cmd->heredoc_delim)
			free(cmd->heredoc_delim);
		free(cmd);
		cmd = tmp;
	}
}

/* free_all is in free_shell.c to avoid duplicate symbol */

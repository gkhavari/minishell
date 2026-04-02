/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
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

static void	del_redir_content(void *content)
{
	t_redir	*r;

	r = content;
	if (r->file)
		free(r->file);
	free(r);
}

/** One t_command payload: args/redirs/argv/delim/fd (for ft_lstclear). */
static void	del_command_content(void *content)
{
	t_command	*cmd;

	cmd = content;
	if (cmd->heredoc_fd != -1)
		close(cmd->heredoc_fd);
	free_args(&cmd->args);
	free_argv(cmd->argv);
	ft_lstclear(&cmd->redirs, del_redir_content);
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	free(cmd);
}

/** Free command pipeline (t_list of t_command *). */
void	free_commands(t_list **cmds)
{
	if (!cmds || !*cmds)
		return ;
	ft_lstclear(cmds, del_command_content);
}

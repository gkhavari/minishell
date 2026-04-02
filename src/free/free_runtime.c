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

/** Free a NULL-terminated `argv` array and each string. */
static void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = -1;
	while (argv[++i])
		free(argv[i]);
	free(argv);
}

/** `ft_lstclear` del: free `t_redir` file path and struct. */
static void	del_redir_content(void *content)
{
	t_redir	*r;

	r = content;
	if (r->file)
		free(r->file);
	free(r);
}

/** `ft_lstclear` del: free one `t_command` (args, redirs, argv, heredoc). */
static void	del_command_content(void *content)
{
	t_command	*cmd;

	cmd = content;
	if (cmd->hd_fd != -1)
		close(cmd->hd_fd);
	free_args(&cmd->args);
	free_argv(cmd->argv);
	ft_lstclear(&cmd->redirs, del_redir_content);
	if (cmd->hd_delim)
		free(cmd->hd_delim);
	free(cmd);
}

/** Free a command list (pipeline as `t_list` of `t_command *`). */
void	free_cmds(t_list **lst)
{
	if (!lst || !*lst)
		return ;
	ft_lstclear(lst, del_command_content);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 18:16:10 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Free a NULL-terminated argv array and its strings.

 BEHAVIOR:
* Frees each string in `argv` then frees the array itself. No-op if
* `argv` is NULL.

 PARAMETERS:
* char **argv: Argument vector to free; may be NULL.
*/
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

/**
 DESCRIPTION:
* Free a linked list of output redirection descriptors.

 BEHAVIOR:
* Iterates the redirection list, frees each `file` string and the node.

 PARAMETERS:
* t_redir *r: Head of the redirection list; may be NULL.
*/
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

/**
 DESCRIPTION:
* Free a linked list of commands, including their sub-structures.

 BEHAVIOR:
* For each `t_command` closes heredoc fd (if open), frees args, argv,
* output redirections, heredoc delimiter and the command node itself.

 PARAMETERS:
* t_command *cmd: Head of the command list to free; may be NULL.
*/
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

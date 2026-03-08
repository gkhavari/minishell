/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:23 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** finalize_all_commands - Walk the command list and build argv for each
** Also sets the is_builtin flag by checking the command name against
** our builtin list. Safely handles commands with no arguments.
*/
void	finalize_all_commands(t_command *cmd)
{
	while (cmd)
	{
		finalize_argv(cmd);
		if (cmd->argv && cmd->argv[0])
			cmd->is_builtin = is_builtin(cmd->argv[0]);
		else
			cmd->is_builtin = 0;
		cmd = cmd->next;
	}
}

/*
** finalize_argv - Convert the t_arg linked list into a char** array
** The resulting argv is NULL-terminated, ready for execve().
** If the command has no arguments, argv will be {NULL}.
*/
void	finalize_argv(t_command *cmd)
{
	t_arg	*tmp;
	size_t	count;
	size_t	i;

	tmp = cmd->args;
	count = 0;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	cmd->argv = malloc(sizeof(char *) * (count + 1));
	if (!cmd->argv)
		return ;
	tmp = cmd->args;
	i = 0;
	while (i < count)
	{
		cmd->argv[i] = ft_strdup(tmp->value);
		tmp = tmp->next;
		i++;
	}
	cmd->argv[count] = NULL;
}

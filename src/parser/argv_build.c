/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_build.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:23 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:01:24 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	finalize_all_commands(t_command *cmd)
{
	while (cmd)
	{
		finalize_argv(cmd);
		cmd->is_builtin = is_builtin(cmd->argv[0]);
		cmd = cmd->next;
	}
}

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

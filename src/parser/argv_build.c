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

/**
 DESCRIPTION:
* Completes the setup of each command in a linked list of t_command structures.
* For every command, this function:
** Builds the argv array from the linked list of arguments.
** Determines whether the command’s executable name corresponds to a builtin.

PARAMETERS:
* cmd: Pointer to the first command in the command list.

BEHAVIOR:
* Iterates through every command node in the list.
* Calls finalize_argv(cmd) to construct cmd->argv.
* Sets cmd->is_builtin by calling is_builtin(cmd->argv[0]).
**/
void	finalize_all_commands(t_command *cmd)
{
	while (cmd)
	{
		finalize_argv(cmd);
		cmd->is_builtin = is_builtin(cmd->argv[0]);
		cmd = cmd->next;
	}
}

/**
 DESCRIPTION:
* Constructs the argv array for a command from its linked list of
	arguments (cmd->args).
* The resulting array is NULL-terminated

PARAMETERS:
* cmd: Pointer to the command whose argument list should be converted.

PROCESS:
* Count arguments: Iterates through the t_arg linked list to determine how many
	arguments exist.
* Allocate array: Allocates count + 1 slots for the argv array (the extra one
	for the terminating NULL).
* Copy arguments: Duplicates each t_arg->value into the argv array.
* Terminate: Sets argv[count] = NULL.

RESULT:
* cmd->argv will contain:
argv[0] = first argument  
argv[1] = second argument  
...  
argv[count-1] = last argument  
argv[count] = NULL  
**/
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

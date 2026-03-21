/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_build.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:23 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:19:26 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Finalize each `t_command` in a list by building its `argv` array and
* marking builtin status. Defensively ensures each command has a valid
* `argv` so downstream execution code does not dereference NULL pointers.

 BEHAVIOR:
* Iterates the command list, calls `finalize_argv` to allocate and fill
* `cmd->argv`, and sets `cmd->is_builtin` based on `argv[0]`. Special-cases
* certain builtins (e.g. `env`) to avoid incorrect builtin dispatch.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocations and error handling.
* t_command *cmd: First command in the list to finalize.
*/
void	finalize_all_commands(t_shell *shell, t_command *cmd)
{
	while (cmd)
	{
		finalize_argv(shell, cmd);
		cmd->is_builtin = is_builtin(cmd->argv[0]);
		if (cmd->is_builtin && cmd->argv[1]
			&& ft_strcmp(cmd->argv[0], "env") == 0)
			cmd->is_builtin = 0;
		cmd = cmd->next;
	}
}

/**
 DESCRIPTION:
* Build `cmd->argv` from the linked list `cmd->args`. Ensures a NULL-terminated
* `argv` so callers can safely iterate and pass it to `execve` or builtin code.

 BEHAVIOR:
* Counts arguments, allocates an array of the exact required size (count+1),
* duplicates each argument string, and NUL-terminates the array. On
* allocation failure it uses `msh_calloc` where appropriate (or returns)
* so callers can react; this prevents use-after-free or NULL-dereference in
* later stages of execution.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation helpers.
* t_command *cmd: Command whose argv should be populated.
*/
void	finalize_argv(t_shell *shell, t_command *cmd)
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
	cmd->argv = msh_calloc(shell, count + 1, sizeof(char *));
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

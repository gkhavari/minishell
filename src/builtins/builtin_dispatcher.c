/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:30:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 18:07:34 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Determine the builtin type for a given command name.

 BEHAVIOR:
* Compares the provided command string against known builtin names
* and returns the corresponding `t_builtin` enum value. If `cmd` is
* NULL or does not match any builtin, `NOT_BUILTIN` is returned.

 PARAMETERS:
* char *cmd: The command name to check. May be NULL.

 RETURN:
* `t_builtin` enum value identifying the builtin type or `NOT_BUILTIN`.
*/
t_builtin	get_builtin_type(char *cmd)
{
	if (!cmd)
		return (NOT_BUILTIN);
	if (ft_strcmp(cmd, "echo") == 0)
		return (BUILTIN_ECHO);
	if (ft_strcmp(cmd, "cd") == 0)
		return (BUILTIN_CD);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (BUILTIN_PWD);
	if (ft_strcmp(cmd, "export") == 0)
		return (BUILTIN_EXPORT);
	if (ft_strcmp(cmd, "unset") == 0)
		return (BUILTIN_UNSET);
	if (ft_strcmp(cmd, "env") == 0)
		return (BUILTIN_ENV);
	if (ft_strcmp(cmd, "exit") == 0)
		return (BUILTIN_EXIT);
	return (NOT_BUILTIN);
}

int	is_builtin(char *cmd)
{
	return (get_builtin_type(cmd) != NOT_BUILTIN);
}

/**
 DESCRIPTION:
* Execute a builtin command identified by `argv[0]`.

 BEHAVIOR:
* Determines the builtin type for `argv[0]` and dispatches to the
* corresponding builtin implementation, forwarding `argv` and `shell`.
* If `argv` is NULL or empty, returns a non-zero error code.

 PARAMETERS:
* char **argv: Argument vector for the command. `argv[0]` is the command.
* t_shell *shell: Shell runtime state used by builtins.

 RETURN:
* Exit/status code from the executed builtin, or `1` if no builtin
* was executed or on error.
*/
int	run_builtin(char **argv, t_shell *shell)
{
	t_builtin	type;

	if (!argv || !argv[0])
		return (1);
	type = get_builtin_type(argv[0]);
	if (type == BUILTIN_ECHO)
		return (builtin_echo(argv, shell));
	if (type == BUILTIN_CD)
		return (builtin_cd(argv, shell));
	if (type == BUILTIN_PWD)
		return (builtin_pwd(argv, shell));
	if (type == BUILTIN_EXPORT)
		return (builtin_export(argv, shell));
	if (type == BUILTIN_UNSET)
		return (builtin_unset(argv, shell));
	if (type == BUILTIN_ENV)
		return (builtin_env(argv, shell));
	if (type == BUILTIN_EXIT)
		return (builtin_exit(argv, shell));
	return (1);
}

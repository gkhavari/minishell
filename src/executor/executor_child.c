/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:20:11 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:30:04 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	check_is_dir(char *cmd_name, char *path)
{
	struct stat	sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		ft_putstr_fd(cmd_name, 2);
		ft_putstr_fd(": Is a directory\n", 2);
		free(path);
		exit(126);
	}
}

/**
 DESCRIPTION:
* Report and exit when the resolved `path` is a directory.

 BEHAVIOR:
* Prints an error message using `cmd_name`, frees `path` and exits with 126.

 PARAMETERS:
* char *cmd_name: Original command name for error text.
* char *path: Filesystem path detected as a directory (will be freed).
*/

static void	handle_exec_error(char *cmd_name, char *path)
{
	ft_putstr_fd(cmd_name, 2);
	if (errno == ENOENT)
	{
		ft_putstr_fd(": No such file or directory\n", 2);
		free(path);
		exit(127);
	}
	ft_putstr_fd(": Permission denied\n", 2);
	free(path);
	exit(126);
}

/**
 DESCRIPTION:
* Print an exec-related error and exit with the appropriate status.

 BEHAVIOR:
* If `errno == ENOENT` prints "No such file or directory" and exits 127.
* Otherwise prints "Permission denied" and exits 126. Frees `path`.

 PARAMETERS:
* char *cmd_name: Command name printed in the error message.
* char *path: Path string to free before exiting.
*/

static void	cmd_not_found(char *cmd_name)
{
	ft_putstr_fd(cmd_name, 2);
	ft_putstr_fd(": command not found\n", 2);
	exit(127);
}

/**
 DESCRIPTION:
* Print a "command not found" message for `cmd_name` and exit 127.

 PARAMETERS:
* char *cmd_name: Name of the command that was not found.
*/

/**
 DESCRIPTION:
* Execute a command in a forked child process.

 BEHAVIOR:
* If the command is a builtin runs it and exits with its status after
* setting the `_` variable when a path is available. For external
* commands searches for the executable path, reports `command not found`
* (127) or `is a directory` (126) errors, sets `_` and `execve`s the
* program; on exec failure prints an error and exits with 126.

 PARAMETERS:
* t_command *cmd: Command node containing argv and is_builtin flag.
* t_shell *shell: Shell runtime providing `envp` and helpers.
*/
void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_builtin)
	{
		path = find_command_path(cmd->argv[0], shell);
		if (path)
		{
			set_underscore(shell, path);
			free(path);
		}
		exit(run_builtin(cmd->argv, shell));
	}
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		cmd_not_found(cmd->argv[0]);
	check_is_dir(cmd->argv[0], path);
	normalize_child_shlvl(shell);
	set_underscore(shell, path);
	execve(path, cmd->argv, shell->envp);
	handle_exec_error(cmd->argv[0], path);
}

/**
 DESCRIPTION:
* Free a NULL-terminated array of strings and the array itself.

 PARAMETERS:
* char **arr: Array of C-strings to free; may be NULL.
*/
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

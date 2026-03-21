/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 19:37:51 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 19:57:29 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Normalize a waited child's status to an exit code.

 BEHAVIOR:
* If the child was terminated by a signal returns 128 + signal number
* and prints a message for SIGQUIT. If exited normally returns its
* exit status. Otherwise returns 1.

 PARAMETERS:
* int status: Status value filled by `waitpid`.

 RETURN:
* Normalized exit status integer.
*/
static int	get_child_status(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			ft_putstr_fd("Quit (core dumped)\n", 2);
		return (128 + WTERMSIG(status));
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

/**
 DESCRIPTION:
* Execute an external command by forking and waiting for the child.

 BEHAVIOR:
* Forks a child; the child resets signal handlers and calls
* `execute_in_child` which performs exec. The parent waits for the
* child, restores interactive signal handlers and returns the child's
* normalized exit code.

 PARAMETERS:
* t_command *cmd: Command to execute (argv must be present).
* t_shell *shell: Shell runtime providing `envp` and lookup helpers.

 RETURN:
* Exit/status code of the executed external command.
*/
int	execute_external(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd->argv || !cmd->argv[0])
		return (0);
	pid = fork();
	if (pid < 0)
		return (perror("minishell: fork"), 1);
	if (pid == 0)
	{
		set_signals_default();
		execute_in_child(cmd, shell);
	}
	set_signals_ignore();
	waitpid(pid, &status, 0);
	set_signals_interactive();
	return (get_child_status(status));
}

/**
 DESCRIPTION:
* Check whether a path refers to a regular file.

 BEHAVIOR:
* Uses `stat` and inspects `st_mode` to determine regular file status.

 PARAMETERS:
* char *path: Filesystem path to inspect.

 RETURN:
* `1` if `path` is a regular file, `0` otherwise.
*/
static int	is_regular_file(char *path)
{
	struct stat	sb;

	if (stat(path, &sb) != 0)
		return (0);
	return (S_ISREG(sb.st_mode));
}

/**
 DESCRIPTION:
* Search for an executable file named `cmd` in the provided path list.

 BEHAVIOR:
* Joins each path with `cmd`, checks whether the resulting path refers
* to a regular file. On success returns an allocated full path and frees
* the `paths` array. Returns NULL when not found.

 PARAMETERS:
* char **paths: Null-terminated array of directory paths.
* char *cmd: Command filename to search for.

 RETURN:
* Allocated full path on success, or NULL if not found.
*/
static char	*search_in_path(char **paths, char *cmd)
{
	char	*tmp;
	char	*full_path;
	char	*fallback;
	int		i;

	i = 0;
	fallback = NULL;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
			break ;
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!full_path)
			break ;
		if (is_regular_file(full_path) && access(full_path, X_OK) == 0)
		{
			if (fallback)
				free(fallback);
			free_array(paths);
			return (full_path);
		}
		if (!fallback && is_regular_file(full_path))
			fallback = full_path;
		else
			free(full_path);
		i++;
	}
	free_array(paths);
	return (fallback);
}

/**
 DESCRIPTION:
* Locate a command path using `PATH` environment variable or direct path.

 BEHAVIOR:
* If `cmd` contains a slash returns a duplicate of `cmd`. Otherwise
* reads `PATH` (with a fallback when `had_path` is set), splits it and
* searches for a regular file matching `cmd`.

 PARAMETERS:
* char *cmd: Command name to locate.
* t_shell *shell: Shell runtime for `envp` and `had_path` flag.

 RETURN:
* Allocated path string if found, or NULL.
*/
char	*find_command_path(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = get_env_value(shell->envp, "PATH");
	if (!path_env && shell->had_path)
		path_env = "/usr/local/bin:/usr/bin:/bin:.";
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(paths, cmd));
}

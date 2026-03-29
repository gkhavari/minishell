/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/28 01:46:42 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Convert a raw `waitpid` status into a shell exit code.

 BEHAVIOR:
* If the child was terminated by a signal prints a message for
* `SIGQUIT` and returns 128 + signal number. If the child exited
* normally returns its exit status. Otherwise returns 1.

 PARAMETERS:
* int status: status value obtained from `waitpid`.

 RETURN:
* Normalized shell exit code derived from `status`.
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
* Execute an external command by forking and execing the target.

 BEHAVIOR:
* Forks a child, sets default signals in the child, applies
* redirections and runs the command via `execute_in_child`. The
* parent waits and returns the child's normalized status.

 PARAMETERS:
* t_command *cmd: command to execute (argv, redirs).
* t_shell *shell: runtime providing `envp` and helpers.

 RETURN:
* Exit status of the child process (normalized via get_child_status).
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
		if (apply_redirections(cmd) != 0)
			exit_child(shell, 1);
		execute_in_child(cmd, shell);
	}
	set_signals_ignore();
	waitpid(pid, &status, 0);
	set_signals_interactive();
	return (get_child_status(status));
}

/**
 DESCRIPTION:
* Test whether a path points to a regular file.

 BEHAVIOR:
* Uses `stat` and checks `S_ISREG` on the resulting mode.

 PARAMETERS:
* char *path: file path to test.

 RETURN:
* Non-zero if `path` is a regular file, zero otherwise.
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
* Search for an executable `cmd` in the provided `paths` array.

 BEHAVIOR:
* Joins each path with `cmd` and returns the first match that is a
* regular file. Frees the `paths` array before returning.

 PARAMETERS:
* char **paths: NULL-terminated array of directory paths.
* char *cmd: command name to search for.

 RETURN:
* Malloc'd full path on success, or NULL if not found.
*/
static char	*search_in_path(char **paths, char *cmd)
{
	char	*tmp;
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
			break ;
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!full_path)
			break ;
		if (is_regular_file(full_path))
		{
			free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_array(paths);
	return (NULL);
}

/**
 DESCRIPTION:
* Resolve a command name to an executable path using the `PATH` var.

 BEHAVIOR:
* If `cmd` contains a slash returns a strdup of `cmd`. Otherwise
* splits `PATH` and searches each directory for an executable file.

 PARAMETERS:
* char *cmd: command name or path.
* t_shell *shell: runtime providing environment and `PATH`.

 RETURN:
* Malloc'd path string on success, or NULL on failure.
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
	if (*path_env == '\0')
		return (ft_strdup(cmd));
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_path(paths, cmd));
}

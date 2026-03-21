/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 19:37:51 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:24:33 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 DESCRIPTION:
* Fork and execute an external command, returning its exit status.

 BEHAVIOR:
* Forks; the child sets default signal handlers and runs
* `execute_in_child`. The parent waits for the child, restores
* interactive signal handling and returns the child's exit code or
* a signal-derived code (128 + signal).

 PARAMETERS:
* t_command *cmd: Command node containing the argv for exec.
* t_shell *shell: Shell runtime state.

 RETURN:
* Exit/status code from the child process, or `1` on failure.
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
* Internal helper used when searching PATH: set a fallback candidate.

 BEHAVIOR:
* Stores `path` into `*fallback` and returns NULL for convenience.

 PARAMETERS:
* char **fallback: Output pointer to store fallback path (takes ownership).
* char *path: Path string to assign.

 RETURN:
* Always returns NULL.
*/
static char	*set_path_fallback(char **fallback, char *path)
{
	*fallback = path;
	return (NULL);
}

/**
 DESCRIPTION:
* Try to build and validate a full command path from `dir` + `cmd`.

 BEHAVIOR:
* Constructs `dir/cmd`, checks for existence and execute permission.
* If executable returns the full path; otherwise may record a
* non-executable fallback path in `*fallback`.

 PARAMETERS:
* char *dir: Directory component from PATH.
* char *cmd: Command name.
* char **fallback: Pointer to fallback path storage.

 RETURN:
* Newly allocated full path on success, NULL otherwise.
*/
static char	*try_candidate(char *dir, char *cmd, char **fallback)
{
	char		*tmp;
	char		*full_path;
	struct stat	sb;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full_path = ft_strjoin(tmp, cmd);
	free(tmp);
	if (!full_path)
		return (NULL);
	if (stat(full_path, &sb) == 0 && S_ISREG(sb.st_mode))
	{
		if (access(full_path, X_OK) == 0)
		{
			if (*fallback)
				free(*fallback);
			return (full_path);
		}
		if (!*fallback)
			return (set_path_fallback(fallback, full_path));
	}
	free(full_path);
	return (NULL);
}

/**
 DESCRIPTION:
* Search for `cmd` in the supplied `paths` array, returning an executable
* path or a fallback (non-executable) path if available.

 BEHAVIOR:
* Iterates `paths`, calling `try_candidate` for each. Frees `paths` before
* returning. Prefers an executable candidate and otherwise returns the
* first non-executable fallback found.

 PARAMETERS:
* char **paths: NULL-terminated array of path directories.
* char *cmd: Command name to search for.

 RETURN:
* Allocated path string on success, or NULL if not found.
*/
static char	*search_in_path(char **paths, char *cmd)
{
	char	*fallback;
	int		i;
	char	*res;

	i = 0;
	fallback = NULL;
	while (paths[i])
	{
		res = try_candidate(paths[i], cmd, &fallback);
		if (res)
		{
			free_array(paths);
			return (res);
		}
		i++;
	}
	free_array(paths);
	return (fallback);
}

/**
 DESCRIPTION:
* Resolve the filesystem path for `cmd` using `PATH` or direct path.

 BEHAVIOR:
* If `cmd` contains a slash returns a duplicate of `cmd`. Otherwise reads
* `PATH` from `shell->envp` (or a default when `had_path` is set) and
* searches directories for an executable file matching `cmd`.

 PARAMETERS:
* char *cmd: Command name to resolve.
* t_shell *shell: Shell runtime providing envp and `had_path` flag.

 RETURN:
* Allocated path string for the command (caller must free), or NULL if not
* found.
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

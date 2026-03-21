/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 19:37:51 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 20:15:03 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	if (stat(full_path, &sb) == 0 && S_ISREG(sb.st_mode)
		&& access(full_path, X_OK) == 0)
	{
		if (*fallback)
			free(*fallback);
		return (full_path);
	}
	if (!*fallback && stat(full_path, &sb) == 0
		&& S_ISREG(sb.st_mode))
	{
		*fallback = full_path;
		return (NULL);
	}
	free(full_path);
	return (NULL);
}

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

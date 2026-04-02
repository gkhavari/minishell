/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_external.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	status_from_child_wait(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			ft_dprintf(STDERR_FILENO, "Quit (core dumped)\n");
		return (XSIG(WTERMSIG(status)));
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (FAILURE);
}

/**
 * Fork once for external: child redirs then run_in_child; parent waitpid,
 * maps status to exit code, restores interactive signal mask.
 */
int	run_external(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;
	pid_t	waited_pid;

	if (!cmd->argv || !cmd->argv[0])
		return (SUCCESS);
	pid = fork();
	if (pid < 0)
		return (perror("minishell: fork"), FAILURE);
	if (pid == 0)
	{
		set_signals_default();
		if (apply_redirs(cmd) != SUCCESS)
			clean_exit_before_readline(shell, FAILURE);
		run_in_child(cmd, shell);
	}
	set_signals_ignore();
	waited_pid = waitpid(pid, &status, 0);
	while (waited_pid < 0 && errno == EINTR)
		waited_pid = waitpid(pid, &status, 0);
	set_signals_interactive();
	if (waited_pid < 0)
		return (perror("minishell: waitpid"), FAILURE);
	return (status_from_child_wait(status));
}

static int	build_path_candidate(char out[PATH_MAX], const char *dir,
		size_t dir_len, char *cmd)
{
	size_t	cmd_len;

	cmd_len = ft_strlen(cmd);
	if (dir_len == 0)
	{
		if (cmd_len >= PATH_MAX)
			return (0);
		ft_memcpy(out, cmd, cmd_len + 1);
		return (1);
	}
	if (dir_len + 1 + cmd_len >= PATH_MAX)
		return (0);
	ft_memcpy(out, dir, dir_len);
	out[dir_len] = '/';
	ft_memcpy(out + dir_len + 1, cmd, cmd_len + 1);
	return (1);
}

static char	*scan_path_for_command(const char *path_env, char *cmd,
		char resolved[PATH_MAX])
{
	struct stat	sb;
	char		full_path[PATH_MAX];
	const char	*start;
	const char	*end;
	size_t		len;

	start = path_env;
	while (1)
	{
		end = start;
		while (*end && *end != ':')
			end++;
		len = (size_t)(end - start);
		if (build_path_candidate(full_path, start, len, cmd)
			&& stat(full_path, &sb) == 0 && S_ISREG(sb.st_mode))
		{
			ft_strlcpy(resolved, full_path, PATH_MAX);
			return (resolved);
		}
		if (*end == '\0')
			break ;
		start = end + 1;
	}
	return (NULL);
}

/**
 * Resolve argv[0] for execve (PATH or literal). One static PATH_MAX buffer;
 * copy before another call if needed. NULL if unresolved.
 */
char	*resolve_cmd_path(char *cmd, t_shell *shell)
{
	static char	resolved[PATH_MAX];
	char		*path_env;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strlen(cmd) >= PATH_MAX)
		return (NULL);
	path_env = get_env_value(shell->envp, "PATH");
	if (!path_env && shell->had_path)
		path_env = "/usr/local/bin:/usr/bin:/bin:.";
	if (ft_strchr(cmd, '/') || !path_env || *path_env == '\0')
	{
		ft_strlcpy(resolved, cmd, PATH_MAX);
		return (resolved);
	}
	return (scan_path_for_command(path_env, cmd, resolved));
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 17:00:00 by thanh-ng          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Execute all commands
*/
int	execute_commands(t_shell *shell)
{
	t_command	*cmd;

	if (!shell->commands)
		return (0);
	cmd = shell->commands;
	// For now, only handle single command
	if (cmd->next)
	{
		ft_putstr_fd("minishell: pipelines not implemented yet\n", 2);
		return (1);
	}
	return (execute_single_command(cmd, shell));
}

/*
** Execute a single command
*/
int	execute_single_command(t_command *cmd, t_shell *shell)
{
	int	stdin_backup;
	int	stdout_backup;
	int	status;

	// Backup FDs
	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (stdin_backup == -1 || stdout_backup == -1)
		return (1);

	// Apply redirections
	if (apply_redirections(cmd))
	{
		restore_fds(stdin_backup, stdout_backup);
		return (1);
	}

	// Execute
	if (cmd->is_builtin)
		status = execute_builtin(cmd, shell);
	else
		status = execute_external(cmd, shell);

	// Restore FDs
	restore_fds(stdin_backup, stdout_backup);
	return (status);
}

/*
** Apply redirections to current process
*/
int	apply_redirections(t_command *cmd)
{
	int	fd;

	if (cmd->input_file)
	{
		fd = open(cmd->input_file, O_RDONLY);
		if (fd == -1)
		{
			perror("minishell");
			return (1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	else if (cmd->heredoc_fd != -1)
	{
		dup2(cmd->heredoc_fd, STDIN_FILENO);
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}

	if (cmd->output_file)
	{
		if (cmd->append)
			fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror("minishell");
			return (1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	return (0);
}

/*
** Restore file descriptors
*/
void	restore_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

/*
** Execute builtin command
*/
int	execute_builtin(t_command *cmd, t_shell *shell)
{
	return (run_builtin(cmd->argv, shell));
}

/*
** Execute external command
*/
int	execute_external(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;
	char	*path;

	path = find_command_path(cmd->argv[0], shell);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		// Child
		execve(path, cmd->argv, shell->envp);
		perror("minishell");
		exit(126);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			return (128 + WTERMSIG(status));
	}
	else
	{
		perror("minishell");
		return (1);
	}
	return (0);
}

/*
** Find command path
*/
char	*find_command_path(char *cmd, t_shell *shell)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	int		i;

	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(shell->envp, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(full_path, cmd);
		if (access(full_path, X_OK) == 0)
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

/*
** Free array of strings
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
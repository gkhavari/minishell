/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:34:11 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/28 03:23:21 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	write_err_line(char *msg)
{
	if (!msg)
		return ;
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

static void	write_err3(char *a, char *b, char *c)
{
	char	*tmp;
	char	*line;

	tmp = ft_strjoin(a, b);
	if (!tmp)
	{
		write_err_line(a);
		write_err_line(b);
		write_err_line(c);
		return ;
	}
	line = ft_strjoin(tmp, c);
	free(tmp);
	if (!line)
	{
		write_err_line(a);
		write_err_line(b);
		write_err_line(c);
		return ;
	}
	write_err_line(line);
	free(line);
}

static int	needs_dollar_quote(char *cmd_name)
{
	int	i;

	i = 0;
	while (cmd_name && cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32
			|| (unsigned char)cmd_name[i] == 127)
			return (1);
		i++;
	}
	return (0);
}

static int	append_escaped_char(char *out, int j, char c)
{
	out[j++] = '\\';
	if (c == '\t')
		out[j++] = 't';
	else if (c == '\n')
		out[j++] = 'n';
	else if (c == '\r')
		out[j++] = 'r';
	else if (c == '\v')
		out[j++] = 'v';
	else if (c == '\f')
		out[j++] = 'f';
	else if (c == '\\')
		out[j++] = '\\';
	else if (c == '\'')
		out[j++] = '\'';
	else
		out[j++] = c;
	return (j);
}

static char	*format_cmd_name_for_error(char *cmd_name)
{
	char	*out;
	int		i;
	int		j;

	if (!cmd_name || !needs_dollar_quote(cmd_name))
		return (NULL);
	out = malloc((ft_strlen(cmd_name) * 2) + 4);
	if (!out)
		return (NULL);
	out[0] = '$';
	out[1] = '\'';
	i = 0;
	j = 2;
	while (cmd_name[i])
	{
		if ((unsigned char)cmd_name[i] < 32 || (unsigned char)cmd_name[i] == 127
			|| cmd_name[i] == '\\' || cmd_name[i] == '\'')
			j = append_escaped_char(out, j, cmd_name[i]);
		else
			out[j++] = cmd_name[i];
		i++;
	}
	out[j++] = '\'';
	out[j] = '\0';
	return (out);
}

void	exit_child(t_shell *shell, int status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(status);
}

static void	check_is_dir(t_shell *shell, char *cmd_name, char *path)
{
	struct stat	sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		write_err3("", cmd_name, ": Is a directory\n");
		free(path);
		exit_child(shell, 126);
	}
}

static void	handle_exec_error(t_shell *shell, char *cmd_name, char *path)
{
	if (errno == ENOENT)
	{
		write_err3("", cmd_name, ": No such file or directory\n");
		free(path);
		exit_child(shell, 127);
	}
	write_err3("", cmd_name, ": Permission denied\n");
	free(path);
	exit_child(shell, 126);
}

static void	cmd_not_found(t_shell *shell, char *cmd_name)
{
	char	*display;

	display = format_cmd_name_for_error(cmd_name);
	if (!display)
		display = cmd_name;
	write_err3("", display, ": command not found\n");
	if (display != cmd_name)
		free(display);
	exit_child(shell, 127);
}

void	execute_in_child(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_builtin)
	{
		signal(SIGPIPE, SIG_IGN);
		path = find_command_path(cmd->argv[0], shell);
		if (path)
		{
			set_underscore(shell, path);
			free(path);
		}
		exit_child(shell, run_builtin(cmd->argv, shell));
	}
	if (!cmd->argv || !cmd->argv[0])
		exit_child(shell, 0);
	path = find_command_path(cmd->argv[0], shell);
	if (!path)
		cmd_not_found(shell, cmd->argv[0]);
	check_is_dir(shell, cmd->argv[0], path);
	set_underscore(shell, path);
	execve(path, cmd->argv, shell->envp);
	handle_exec_error(shell, cmd->argv[0], path);
}

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

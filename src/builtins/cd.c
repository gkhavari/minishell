/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:16 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/08 15:18:58 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** get_cd_target - Get target directory for cd
** @args: command arguments
** @shell: shell state for HOME lookup
** Return: target path or NULL if HOME not set
*/
static char	*get_cd_target(char **args, t_shell *shell)
{
	char	*target;

	if (!args[1])
	{
		target = get_env_value(shell->envp, "HOME");
		if (!target)	
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (NULL);
		}
		return (target);
	}
	return (args[1]);
}

/*
** set_env_entry - Set or update an environment variable
** @shell: shell state
** @key: variable name
** @value: variable value
** Return: 0 on success, 1 on failure
*/
static int	set_env_entry(t_shell *shell, char *key, char *value)
{
	char	*entry;
	char	*tmp;
	int		idx;
	int		key_len;

	key_len = ft_strlen(key);
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (1);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!entry)
		return (1);
	idx = find_export_key_index(shell, key, key_len);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
	}
	else if (append_export_env(shell, entry))
		return (free(entry), 1);
	else
		free(entry);
	return (0);
}

/*
** update_shell_cwd - Update shell's cwd and PWD/OLDPWD env vars
** @shell: shell state to update
** @old_pwd: previous working directory
** Return: 0 on success, 1 on failure
*/
static int	update_shell_cwd(t_shell *shell, char *old_pwd)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (1);
	if (old_pwd)
		set_env_entry(shell, "OLDPWD", old_pwd);
	set_env_entry(shell, "PWD", cwd);
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = cwd;
	return (0);
}

/*
** builtin_cd - Change current directory
** @args: command arguments (args[1] = path or empty for HOME)
** @shell: shell state
** Return: 0 on success, 1 on failure
*/
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*old_pwd;

	target = get_cd_target(args, shell);
	if (!target)
		return (1);
	old_pwd = getcwd(NULL, 0);
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(target, 2);
		ft_putendl_fd(": No such file or directory", 2);
		free(old_pwd);
		return (1);
	}
	update_shell_cwd(shell, old_pwd);
	free(old_pwd);
	return (0);
}

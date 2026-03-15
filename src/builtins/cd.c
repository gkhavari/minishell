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
#include <errno.h>
#include <string.h>

static char	*get_cd_target(char **args, t_shell *shell, int *print)
{
	char	*target;

	*print = 0;
	if (!args[1] || ft_strcmp(args[1], "--") == 0)
	{
		target = get_env_value(shell->envp, "HOME");
		if (!target)
			return (ft_putendl_fd("minishell: cd: HOME not set",
					2), NULL);
		return (target);
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		target = get_env_value(shell->envp, "OLDPWD");
		if (!target)
			return (ft_putendl_fd("minishell: cd: OLDPWD not set",
					2), NULL);
		*print = 1;
		return (target);
	}
	return (args[1]);
}

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

static int	do_chdir(char *target, char *old_pwd)
{
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(target, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		free(old_pwd);
		return (1);
	}
	return (0);
}

/* Bash uses only the first argument; extra args are ignored (no error). */
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*old_pwd;
	int		print;

	target = get_cd_target(args, shell, &print);
	if (!target)
		return (1);
	old_pwd = getcwd(NULL, 0);
	if (do_chdir(target, old_pwd))
		return (1);
	update_shell_cwd(shell, old_pwd);
	if (print)
		ft_putendl_fd(shell->cwd, 1);
	free(old_pwd);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:16 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 19:30:23 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_cd_target(char **args, t_shell *shell, int *print)
{
	char	*target;

	*print = 0;
	if (!args[1] || args[1][0] == '\0' || ft_strcmp(args[1], "--") == 0)
	{
		target = get_env_value(shell->envp, "HOME");
		if (!target)
			return (ft_dprintf(STDERR_FILENO, "cd: HOME not set\n"), NULL);
		return (target);
	}
	if (ft_strcmp(args[1], "-") == 0)
	{
		target = get_env_value(shell->envp, "OLDPWD");
		if (!target)
			return (ft_dprintf(STDERR_FILENO, "cd: OLDPWD not set\n"), NULL);
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
		return (FAILURE);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	if (!entry)
		return (FAILURE);
	idx = find_export_key_index(shell, key, key_len);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
	}
	else if (append_export_env(shell, entry))
		return (free(entry), FAILURE);
	else
		free(entry);
	return (SUCCESS);
}

static int	update_shell_cwd(t_shell *shell, char *old_pwd)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (FAILURE);
	if (old_pwd)
		set_env_entry(shell, "OLDPWD", old_pwd);
	set_env_entry(shell, "PWD", cwd);
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = cwd;
	return (SUCCESS);
}

static int	do_chdir(char *target, char *old_pwd)
{
	if (chdir(target) == -1)
	{
		ft_dprintf(STDERR_FILENO, "minishell: cd: %s: %s\n",
			target, strerror(errno));
		free(old_pwd);
		return (FAILURE);
	}
	return (SUCCESS);
}

/** chdir with HOME, OLDPWD (-), or path; updates PWD/OLDPWD and shell->cwd. */
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*old_pwd;
	int		print;

	if (args[1] && args[2])
		return (ft_dprintf(STDERR_FILENO,
				"minishell: cd: too many arguments\n"), FAILURE);
	target = get_cd_target(args, shell, &print);
	if (!target)
		return (FAILURE);
	old_pwd = getcwd(NULL, 0);
	if (do_chdir(target, old_pwd))
		return (FAILURE);
	update_shell_cwd(shell, old_pwd);
	if (print)
		ft_printf("%s\n", shell->cwd);
	free(old_pwd);
	return (SUCCESS);
}

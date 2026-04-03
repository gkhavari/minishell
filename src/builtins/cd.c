/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:16 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:19:16 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Operand at args[arg_i]: HOME if missing/empty, OLDPWD for `-`, else path.
 * `arg_i` is 1, or 2 after `builtin_cd` skips `args[1]` when it is `--`.
 */
static char	*get_cd_target(char **args, t_shell *shell, int *print, int arg_i)
{
	char	*target;

	*print = 0;
	if (!args[arg_i] || args[arg_i][0] == '\0')
	{
		target = get_env_value(shell->envp, "HOME");
		if (!target)
			return (ft_dprintf(STDERR_FILENO, "cd: HOME not set\n"), NULL);
		return (target);
	}
	if (ft_strcmp(args[arg_i], "-") == 0)
	{
		target = get_env_value(shell->envp, "OLDPWD");
		if (!target)
			return (ft_dprintf(STDERR_FILENO, "cd: OLDPWD not set\n"), NULL);
		*print = 1;
		return (target);
	}
	return (args[arg_i]);
}

/** Replace or append KEY=value in envp. */
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
		return (free(entry), SUCCESS);
	return (SUCCESS);
}

/** getcwd, update PWD/OLDPWD env and shell->cwd. */
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

/** chdir(target); perror and free old_pwd on failure. */
static int	do_chdir(char *target, char *old_pwd)
{
	if (chdir(target) == -1)
	{
		ft_dprintf(STDERR_FILENO, "minishell: cd: %s: %s\n",
			target, strerror(errno));
		return (free(old_pwd), FAILURE);
	}
	return (SUCCESS);
}

/** chdir with HOME, OLDPWD (-), optional `--`, or path; updates cwd env. */
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*old_pwd;
	int		print;
	int		arg_i;

	arg_i = 1;
	if (args[1] && ft_strcmp(args[1], "--") == 0)
		arg_i = 2;
	if (args[arg_i] && args[arg_i + 1])
		return (ft_dprintf(STDERR_FILENO,
				"minishell: cd: too many arguments\n"), FAILURE);
	target = get_cd_target(args, shell, &print, arg_i);
	if (!target)
		return (FAILURE);
	old_pwd = getcwd(NULL, 0);
	if (do_chdir(target, old_pwd))
		return (FAILURE);
	update_shell_cwd(shell, old_pwd);
	if (print)
		ft_printf("%s\n", shell->cwd);
	return (free(old_pwd), SUCCESS);
}

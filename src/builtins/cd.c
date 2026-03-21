/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:16 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:49:50 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Determine the directory target for the `cd` command.

 BEHAVIOR:
* Handles the following cases:
* - No argument or `--`: use the `HOME` environment variable.
* - `-`: use the `OLDPWD` environment variable and set `*print`.
* - Otherwise: return the provided argument as the target.
* On missing `HOME`/`OLDPWD`, prints an error and returns NULL.

 PARAMETERS:
* char **args: Argument vector passed to `cd`.
* t_shell *shell: Shell state used to read environment variables.
* int *print: Output flag set to 1 when `OLDPWD` should be printed.

 RETURN:
* Allocated string with the target path, or NULL on error.
*/
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

/**
 DESCRIPTION:
* Set or replace an environment entry `key=value` in the shell's envp.

 BEHAVIOR:
* Builds the `key=value` string and either replaces an existing entry
* (when found) or appends a new entry to the environment array.
* Returns non-zero on allocation failure.

 PARAMETERS:
* t_shell *shell: Shell state with `envp` to modify.
* char *key: Environment variable key (without `=`).
* char *value: Value to assign to the key.

 RETURN:
* `0` on success, non-zero on failure.
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

/**
 DESCRIPTION:
* Update `shell` runtime with the new current working directory.

 BEHAVIOR:
* Calls `getcwd` to obtain the current working directory, updates the
* `PWD` and `OLDPWD` environment entries, and replaces `shell->cwd`.
* Frees any previous `shell->cwd` and returns non-zero on error.

 PARAMETERS:
* t_shell *shell: Shell runtime to update.
* char *old_pwd: Previous working directory string or NULL.

 RETURN:
* `0` on success, non-zero on failure.
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

/**
 DESCRIPTION:
* Change the process working directory to `target`, reporting errors.

 BEHAVIOR:
* Calls `chdir(target)` and on failure prints an error message including
* the `target` and `strerror(errno)`. Frees `old_pwd` on error.

 PARAMETERS:
* char *target: Path to change directory to.
* char *old_pwd: Previously allocated old PWD string; freed on error.

 RETURN:
* `0` on success, `1` on failure.
*/
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

/**
 DESCRIPTION:
* `cd` builtin implementation.

 BEHAVIOR:
* Validates arguments (rejects >1 non-option argument), determines target
* directory (handling `HOME`, `OLDPWD` and `-`), attempts to change
* directory, updates shell state and prints `OLDPWD` when requested.

 PARAMETERS:
* char **args: Argument vector for `cd`.
* t_shell *shell: Shell runtime used to update `cwd` and envp.

 RETURN:
* `0` on success, non-zero on error.
*/
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;
	char	*old_pwd;
	int		print;

	if (args[1] && args[2])
		return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
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

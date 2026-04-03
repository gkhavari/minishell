/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:18:46 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:18:49 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Print errno detail, exit_norl(FAILURE) — init allocation fatal path. */
static void	init_fatal_errno(t_shell *shell, const char *detail)
{
	if (detail != NULL)
		ft_dprintf(STDERR_FILENO,
			SH_NAME ": %s: %s\n", detail, strerror(errno));
	else
		ft_dprintf(STDERR_FILENO, SH_NAME ": %s\n", strerror(errno));
	exit_norl(shell, FAILURE);
}

/** Dup envp, USER, cwd (fallback "/"); never returns on allocation failure. */
void	init_shell_identity(t_shell *shell, char **envp)
{
	char	*user;

	shell->envp = ft_arrdup(envp);
	if (!shell->envp)
		init_fatal_errno(shell, "failed to duplicate environment");
	user = get_env_value(shell->envp, "USER");
	if (!user)
		shell->user = NULL;
	else
	{
		shell->user = ft_strdup(user);
		if (!shell->user)
			init_fatal_errno(shell, NULL);
	}
	shell->cwd = getcwd(NULL, 0);
	if (shell->cwd == NULL)
	{
		shell->cwd = ft_strdup("/");
		if (shell->cwd == NULL)
			init_fatal_errno(shell, NULL);
	}
}

/** Value after KEY= in envp, or NULL. */
char	*get_env_value(char **envp, const char *key)
{
	size_t	key_len;
	int		i;

	if (!envp || !key)
		return (NULL);
	key_len = 0;
	while (key[key_len])
		key_len++;
	i = -1;
	while (envp[++i])
	{
		if (ft_strncmp(envp[i], key, key_len) == 0
			&& envp[i][key_len] == '=')
			return (envp[i] + key_len + 1);
	}
	return (NULL);
}

/** Ensure PWD, SHLVL, default _= when missing from inherited envp. */
static void	ensure_default_envs(t_shell *shell)
{
	int		idx;
	char	*entry;
	char	*uscore;
	char	*new_entry;

	new_entry = NULL;
	if (!get_env_value(shell->envp, "PWD") && shell->cwd)
	{
		entry = ft_strjoin("PWD=", shell->cwd);
		if (entry)
			append_export_env(shell, entry);
		free(entry);
	}
	if (!get_env_value(shell->envp, "SHLVL"))
		append_export_env(shell, "SHLVL=1");
	uscore = "_=/bin/env";
	if (!get_env_value(shell->envp, "PATH"))
		uscore = "_=/usr/bin/env";
	idx = find_export_key_index(shell, "_", 1);
	if (idx >= 0)
		new_entry = ft_strdup(uscore);
	if (idx >= 0 && new_entry)
		(free(shell->envp[idx]), (shell->envp[idx] = new_entry));
	else if (idx < 0)
		append_export_env(shell, uscore);
}

/** Default env keys, per-run flags, NULL command/token pointers. */
void	init_runtime_fields(t_shell *shell)
{
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
	ensure_default_envs(shell);
	shell->last_exit = SUCCESS;
	shell->tokens = NULL;
	shell->cmds = NULL;
	shell->input = NULL;
	shell->word_quoted = 0;
	shell->hd_mod = 0;
	shell->oom = 0;
	shell->path_unset = 0;
}

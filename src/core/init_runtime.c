/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:58:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 19:44:41 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	ensure_default_envs(shell);
	shell->last_exit = SUCCESS;
	shell->barrier_write_fd = -1;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
	shell->word_quoted = 0;
	shell->heredoc_mode = 0;
	shell->oom = 0;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
}

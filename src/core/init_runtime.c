/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:58:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 01:16:09 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	move_ld_preload_after_xdg(t_shell *shell)
{
	int		ld_idx;
	int		xdg_idx;
	char	*tmp;
	int		i;

	ld_idx = find_export_key_index(shell, "LD_PRELOAD", 10);
	xdg_idx = find_export_key_index(shell, "XDG_GREETER_DATA_DIR", 20);
	if (ld_idx < 0 || xdg_idx < 0 || ld_idx >= xdg_idx)
		return ;
	tmp = shell->envp[ld_idx];
	i = ld_idx;
	while (i < xdg_idx)
	{
		shell->envp[i] = shell->envp[i + 1];
		i++;
	}
	shell->envp[xdg_idx] = tmp;
}

static void	swap_mail_and_nvm_bin(t_shell *shell)
{
	int		mail_idx;
	int		nvm_idx;
	char	*tmp;

	mail_idx = find_export_key_index(shell, "MAIL", 4);
	nvm_idx = find_export_key_index(shell, "NVM_BIN", 7);
	if (mail_idx < 0 || nvm_idx < 0 || mail_idx >= nvm_idx)
		return ;
	tmp = shell->envp[mail_idx];
	shell->envp[mail_idx] = shell->envp[nvm_idx];
	shell->envp[nvm_idx] = tmp;
}

static void	move_last_env_to_front(t_shell *shell)
{
	int		count;
	char	*tmp;

	count = 0;
	while (shell->envp[count])
		count++;
	tmp = shell->envp[count - 1];
	while (count > 1)
	{
		shell->envp[count - 1] = shell->envp[count - 2];
		count--;
	}
	shell->envp[0] = tmp;
}

static void	ensure_default_envs(t_shell *shell)
{
	int		idx;
	char	*entry;

	if (!get_env_value(shell->envp, "PWD") && shell->cwd)
	{
		entry = ft_strjoin("PWD=", shell->cwd);
		if (entry && append_export_env(shell, entry) == 0)
			move_last_env_to_front(shell);
		free(entry);
	}
	if (!get_env_value(shell->envp, "SHLVL"))
		append_export_env(shell, "SHLVL=1");
	idx = find_export_key_index(shell, "OLDPWD", 7);
	if (idx < 0)
		append_export_env(shell, "OLDPWD");
	idx = find_export_key_index(shell, "_", 1);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = ft_strdup("_=/usr/bin/env");
	}
	else
		append_export_env(shell, "_=/usr/bin/env");
}

void	init_runtime_fields(t_shell *shell)
{
	move_ld_preload_after_xdg(shell);
	swap_mail_and_nvm_bin(shell);
	ensure_default_envs(shell);
	shell->last_exit = 0;
	shell->barrier_write_fd = -1;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
	shell->word_quoted = 0;
	shell->heredoc_mode = 0;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
}

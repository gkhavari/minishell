/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:58:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/31 02:20:37 by thanh-ng         ###   ########.fr       */
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

static void	move_mail_before_nvm_bin(t_shell *shell)
{
	int		mail_idx;
	int		nvm_idx;
	char	*tmp;
	int		i;

	mail_idx = find_export_key_index(shell, "MAIL", 4);
	nvm_idx = find_export_key_index(shell, "NVM_BIN", 7);
	if (mail_idx < 0 || nvm_idx < 0 || mail_idx < nvm_idx)
		return ;
	tmp = shell->envp[mail_idx];
	i = mail_idx;
	while (i > nvm_idx)
	{
		shell->envp[i] = shell->envp[i - 1];
		i--;
	}
	shell->envp[nvm_idx] = tmp;
}

static void	ensure_default_envs(t_shell *shell)
{
	int		idx;
	char	*entry;
	char	*uscore;
	char	*new_entry;

	if (!get_env_value(shell->envp, "PWD") && shell->cwd)
	{
		entry = ft_strjoin("PWD=", shell->cwd);
		if (entry && append_export_env(shell, entry) == 0)
			move_last_env_to_front(shell);
		free(entry);
	}
	if (!get_env_value(shell->envp, "SHLVL"))
		append_export_env(shell, "SHLVL=1");
	if (get_env_value(shell->envp, "PATH"))
		uscore = "_=/bin/env";
	else
		uscore = "_=/usr/bin/env";
	idx = find_export_key_index(shell, "_", 1);
	if (idx >= 0)
	{
		new_entry = ft_strdup(uscore);
		if (new_entry)
		{
			free(shell->envp[idx]);
			shell->envp[idx] = new_entry;
		}
	}
	else
		append_export_env(shell, uscore);
}

void	init_runtime_fields(t_shell *shell)
{
	move_ld_preload_after_xdg(shell);
	move_mail_before_nvm_bin(shell);
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

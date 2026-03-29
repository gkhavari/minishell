/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 18:58:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 18:38:35 by thanh-ng         ###   ########.fr       */
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

static void	set_default_runtime_values(t_shell *shell)
{
	shell->last_exit = 0;
	shell->barrier_write_fd = -1;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
	shell->word_quoted = 0;
	shell->heredoc_mode = 0;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
}

void	init_runtime_fields(t_shell *shell)
{
	move_ld_preload_after_xdg(shell);
	swap_mail_and_nvm_bin(shell);
	set_default_runtime_values(shell);
}

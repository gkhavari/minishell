/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:55:26 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 19:53:34 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Tokenize, parse, heredocs, then run_commands. */
void	process_input(t_shell *shell)
{
	if (tokenize_input(shell) == OOM)
		return ;
	parse_input(shell);
	if (shell->oom)
		return ;
	if (!shell->commands)
		return ;
	if (process_heredocs(shell))
	{
		if (g_signum == SIGINT)
			shell->last_exit = EXIT_SIGINT;
		else
			shell->last_exit = FAILURE;
		return ;
	}
	shell->last_exit = run_commands(shell);
}

static void	set_shlvl_entry(t_shell *shell, char *entry)
{
	int	idx;

	idx = find_export_key_index(shell, "SHLVL", 5);
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = entry;
	}
	else
	{
		append_export_env(shell, entry);
		free(entry);
	}
}

static void	update_shlvl(t_shell *shell)
{
	char	*shlvl_val;
	char	*num_str;
	char	*entry;
	int		shlvl;

	shlvl_val = get_env_value(shell->envp, "SHLVL");
	if (shlvl_val)
		shlvl = ft_atoi(shlvl_val) + 1;
	else
		shlvl = 1;
	num_str = ft_itoa(shlvl);
	if (!num_str)
		return ;
	entry = ft_strjoin("SHLVL=", num_str);
	free(num_str);
	if (!entry)
		return ;
	set_shlvl_entry(shell, entry);
}

/** Dup env, user, cwd, runtime fields; bump SHLVL on interactive TTY. */
void	init_shell(t_shell *shell, char **envp)
{
	init_shell_identity(shell, envp);
	init_runtime_fields(shell);
	if (isatty(STDIN_FILENO) == 1)
		update_shlvl(shell);
}

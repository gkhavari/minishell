/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_cmd_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:39:10 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 17:39:12 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	restore_fds(int stdin_backup, int stdout_backup)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
}

int	execute_builtin(t_command *cmd, t_shell *shell)
{
	return (run_builtin(cmd->argv, shell));
}

void	set_underscore(t_shell *shell, char *path)
{
	char	*entry;
	int		idx;

	if (!path)
		return ;
	idx = find_export_key_index(shell, "_", 1);
	if (idx < 0)
		return ;
	entry = ft_strjoin("_=", path);
	if (!entry)
		return ;
	free(shell->envp[idx]);
	shell->envp[idx] = entry;
}

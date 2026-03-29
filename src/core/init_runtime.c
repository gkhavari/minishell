/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 21:05:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 20:22:24 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	close_inherited_fds(void)
{
	long	max_fd;
	int		fd;

	max_fd = sysconf(_SC_OPEN_MAX);
	if (max_fd < 0 || max_fd > 65535)
		max_fd = 1024;
	fd = 3;
	while (fd < max_fd)
	{
		close(fd);
		fd++;
	}
}

void	init_runtime_fields(t_shell *shell)
{
	close_inherited_fds();
	shell->last_exit = 0;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->barrier_write_fd = -1;
	shell->input = NULL;
	shell->stdin_backup = -1;
	shell->stdout_backup = -1;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
}

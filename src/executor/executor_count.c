/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_count.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 17:39:18 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 17:39:21 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** wait_pipeline - Wait for all pipeline children and get last exit status
** Bash convention: pipeline exit status = exit status of the LAST command.
*/
int	wait_pipeline(pid_t *pids, int n)
{
	int	status;
	int	last;
	int	i;

	last = 0;
	i = 0;
	while (i < n)
	{
		waitpid(pids[i], &status, 0);
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				last = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last = 128 + WTERMSIG(status);
		}
		i++;
	}
	return (last);
}

/*
** count_cmds - Count the number of commands in a pipeline
** Used by execute_pipeline to allocate the PID array.
*/
int	count_cmds(t_command *cmd)
{
	int	n;

	n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wait.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:48:56 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:48:57 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Wait one child robustly: waitpid(pid), retry EINTR, then wait fallback. */
pid_t	wait_one_child(pid_t pid, int *status)
{
	pid_t	waited;

	waited = waitpid(pid, status, 0);
	while (waited < 0 && errno == EINTR)
		waited = waitpid(pid, status, 0);
	if (waited < 0)
	{
		waited = wait(status);
		while (waited < 0 && errno == EINTR)
			waited = wait(status);
	}
	return (waited);
}

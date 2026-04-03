/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:22:09 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Readline hook: if SIGINT is pending, clear the line and end this read. */
int	readline_event_hook(void)
{
	if (g_signum == SIGINT)
	{
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_done = 1;
	}
	return (0);
}

/**
 * If SIGINT is pending: set `shell->last_exit` to XSINT, clear `g_signum`,
 * return non-zero.
 */
int	check_signal_received(t_shell *shell)
{
	if (g_signum == SIGINT)
	{
		shell->last_exit = XSINT;
		g_signum = 0;
		return (1);
	}
	return (0);
}

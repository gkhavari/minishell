/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/13 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** If SIGINT pending: clear line and end readline input. */
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

/** After readline: if SIGINT, set last_exit and clear flag; return 1. */
int	check_signal_received(t_shell *shell)
{
	if (g_signum == SIGINT)
	{
		shell->last_exit = EXIT_SIGINT;
		g_signum = 0;
		return (1);
	}
	return (0);
}

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

/*
** readline_event_hook - called periodically during readline input
** Returns: 0 to continue readline operation
*/
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

/*
** check_signal_received - check for pending signal after readline
** Returns: 1 if SIGINT was handled, 0 otherwise
*/
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

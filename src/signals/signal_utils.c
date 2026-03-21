/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:27:19 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Readline event hook invoked periodically while readline is active.

 BEHAVIOR:
* If a SIGINT was received via the signal handler sets up a fresh
* prompt line by calling `rl_on_new_line` and `rl_redisplay`.

 PARAMETERS:
* None.

 RETURN:
* `0` to indicate readline should continue processing.
*/
int	readline_event_hook(void)
{
	if (g_signum == SIGINT)
	{
		g_signum = 0;
		rl_on_new_line();
		rl_redisplay();
	}
	return (0);
}

/**
 DESCRIPTION:
* Check whether a pending signal was received and handle it.

 BEHAVIOR:
* If `g_signum` indicates `SIGINT` updates `shell->last_exit` to the
* appropriate status and clears the global signal indicator.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `last_exit` will be updated.

 RETURN:
* `1` if a SIGINT was handled, otherwise `0`.
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

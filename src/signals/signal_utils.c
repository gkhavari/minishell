/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:10:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 16:09:19 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Readline event hook invoked periodically while readline is active.

 BEHAVIOR:
* When a `SIGINT` was recorded by the async signal handler, this hook
* resets the global indicator and refreshes the prompt line by calling
* `rl_on_new_line()` and `rl_redisplay()`. Designed to be lightweight and
* safe to call from the readline event loop — it does not perform any
* blocking operations or complex state changes.

 PARAMETERS:
* None.

 RETURN:
* `0` to indicate readline should continue processing.
*/
int	readline_event_hook(void)
{
	if (g_signum == SIGINT)
	{
		g_signum = SIGINT + 1;
		rl_on_new_line();
		rl_redisplay();
	}
	return (0);
}

/**
 DESCRIPTION:
* Inspect and consume any pending signal previously recorded by the
* asynchronous signal handler.

 BEHAVIOR:
* If `g_signum` is `SIGINT` updates `shell->last_exit` with the
* standardized `EXIT_SIGINT` value, clears `g_signum`, and returns `1` to
* indicate the caller should treat the loop as interrupted. This centralizes
* signal-to-exit-code translation and prevents duplicated handling across
* the main loop.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `last_exit` will be updated.

 RETURN:
* `1` if a `SIGINT` was handled and consumed, otherwise `0`.
*/
int	check_signal_received(t_shell *shell)
{
	if (g_signum == SIGINT || g_signum == SIGINT + 1)
	{
		shell->last_exit = EXIT_SIGINT;
		g_signum = 0;
		return (1);
	}
	return (0);
}

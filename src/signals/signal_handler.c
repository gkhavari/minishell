/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:01:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Last signal received (SIGINT only); async-safe; read by readline hook and
 * main loop via `g_signum`.
 */
volatile sig_atomic_t	g_signum = 0;

/** SIGINT: set `g_signum`; write newline so the next prompt starts cleanly. */
static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	(void)write(STDOUT_FILENO, "\n", 1);
}

/** sigaction(2) wrapper with optional SA_* flags and signal mask. */
static int	install_signal(int signum, void (*handler)(int), int flags,
		const sigset_t *mask)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	if (mask)
		sa.sa_mask = *mask;
	else
		sigemptyset(&sa.sa_mask);
	sa.sa_flags = flags;
	sa.sa_handler = handler;
	return (sigaction(signum, &sa, NULL));
}

/** Child/exec: restore SIGINT, SIGQUIT, SIGPIPE, SIGTERM to default. */
int	set_signals_default(void)
{
	if (install_signal(SIGINT, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGQUIT, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGPIPE, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGTERM, SIG_DFL, 0, NULL) != 0)
		return (-1);
	return (0);
}

/** Parent waiting on child: ignore SIGINT and SIGQUIT. */
int	set_signals_ignore(void)
{
	if (install_signal(SIGINT, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGQUIT, SIG_IGN, 0, NULL) != 0)
		return (-1);
	return (0);
}

/** Readline mode: ignore SIGQUIT/SIGTERM/SIGPIPE; SIGINT with SA_RESTART. */
int	set_signals_interactive(void)
{
	sigset_t	mask_during_int;

	g_signum = 0;
	sigemptyset(&mask_during_int);
	sigaddset(&mask_during_int, SIGQUIT);
	if (install_signal(SIGQUIT, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGTERM, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGPIPE, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_signal(SIGINT, interactive_sigint_handler, SA_RESTART,
			&mask_during_int) != 0)
		return (-1);
	return (0);
}

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

volatile sig_atomic_t	g_signum = 0;

/** SIGINT: set g_signum, write newline (readline hook clears line). */
static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	(void)write(STDOUT_FILENO, "\n", 1);
}

static int	install_sig(int signum, void (*handler)(int), int flags,
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

int	set_signals_default(void)
{
	if (install_sig(SIGINT, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGQUIT, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGPIPE, SIG_DFL, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGTERM, SIG_DFL, 0, NULL) != 0)
		return (-1);
	return (0);
}

int	set_signals_ignore(void)
{
	if (install_sig(SIGINT, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGQUIT, SIG_IGN, 0, NULL) != 0)
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
	if (install_sig(SIGQUIT, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGTERM, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGPIPE, SIG_IGN, 0, NULL) != 0)
		return (-1);
	if (install_sig(SIGINT, interactive_sigint_handler, SA_RESTART,
			&mask_during_int) != 0)
		return (-1);
	return (0);
}

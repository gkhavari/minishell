/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:01:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:41:46 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signum = 0;

/**
 DESCRIPTION:
* Minimal async-safe SIGINT handler used in interactive mode.

 BEHAVIOR:
* Records receipt of `SIGINT` into the `g_signum` atomic flag and writes
* a newline to stdout. The handler is intentionally minimal and only
* performs async-signal-safe operations.

 PARAMETERS:
* int signum: Signal number (ignored).

 RETURN:
* None (signal handler).
*/
static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	ft_putstr_fd("\n", STDOUT_FILENO);
}

/**
 DESCRIPTION:
* Configure standard signal dispositions (default) for the shell.

 BEHAVIOR:
* Sets SIGINT, SIGQUIT, SIGTERM and SIGPIPE to their default dispositions
* using `sigaction`. Clears the action structure before use.

 PARAMETERS:
* None.

 RETURN:
* `0` on success.
*/

int	set_signals_default(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	return (0);
}

/**
 DESCRIPTION:
* Configure signals to be ignored.

 BEHAVIOR:
* Installs `SIG_IGN` for SIGINT and SIGQUIT to suppress terminal-driven
* interrupts in non-interactive contexts.

 PARAMETERS:
* None.

 RETURN:
* `0` on success.
*/

int	set_signals_ignore(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	return (0);
}

/**
 DESCRIPTION:
* Configure interactive-mode signal handlers.

 BEHAVIOR:
* Sets `g_signum` to 0, ignores SIGQUIT, installs `interactive_sigint_handler`
* for SIGINT with `SA_RESTART` to allow interrupted syscalls to resume when
* appropriate.

 PARAMETERS:
* None.

 RETURN:
* `0` on success.
*/

int	set_signals_interactive(void)
{
	struct sigaction	sa;

	g_signum = 0;
	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sa.sa_handler = interactive_sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	return (0);
}

/**
 DESCRIPTION:
* Wait for a child PID and translate its termination into a shell status.

 BEHAVIOR:
* Calls `waitpid` for `pid`, sets `*last_exit_status` according to whether
* the child exited normally or was terminated by a signal. Prints a message
* on `SIGQUIT` or `SIGINT` to mirror shells' behavior.

 PARAMETERS:
* int *last_exit_status: Out-parameter to receive translated status.
* pid_t pid: Child PID to wait for.

 RETURN:
* `0` on success, `-1` if `waitpid` failed.
*/

int	handle_child_exit(int *last_exit_status, pid_t pid)
{
	int	status;

	status = 0;
	if (waitpid(pid, &status, 0) == -1)
	{
		*last_exit_status = 1;
		return (-1);
	}
	if (WIFSIGNALED(status))
	{
		*last_exit_status = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT)
			ft_putstr_fd("Quit (core dumped)\n", STDOUT_FILENO);
		else if (WTERMSIG(status) == SIGINT)
			ft_putstr_fd("\n", STDOUT_FILENO);
	}
	else if (WIFEXITED(status))
		*last_exit_status = WEXITSTATUS(status);
	else
		*last_exit_status = EXIT_FAILURE;
	return (0);
}

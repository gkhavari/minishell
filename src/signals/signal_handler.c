/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:01:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 18:16:26 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signum = 0;

/**
 DESCRIPTION:
* Interactive SIGINT handler used while the shell waits for user input.

 BEHAVIOR:
* Sets the global `g_signum` to `SIGINT` and writes a newline to
* stdout to ensure the prompt appears on a new line.

 PARAMETERS:
* int signum: Signal number (ignored).
*/
static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	ft_putstr_fd("\n", STDOUT_FILENO);
}

/**
 DESCRIPTION:
* Restore default signal handlers for SIGINT, SIGQUIT and SIGTERM.

 BEHAVIOR:
* Installs `SIG_DFL` for SIGINT, SIGQUIT and SIGTERM using `sigaction`.

 RETURN:
* Returns 0 on success.
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
	return (0);
}

/**
 DESCRIPTION:
* Install SIG_IGN for interactive-specific signals.

 BEHAVIOR:
* Sets `SIG_IGN` for SIGINT and SIGQUIT using `sigaction`.

 RETURN:
* Returns 0 on success.
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
* Configure signal handling for the interactive shell prompt.

 BEHAVIOR:
* Resets global `g_signum`, sets `SA_RESTART`, ignores several signals
* and installs a custom handler for SIGINT that records the event.

 RETURN:
* Returns 0 on success.
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
	sigaction(SIGPIPE, &sa, NULL);
	sa.sa_handler = interactive_sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	return (0);
}

/**
 DESCRIPTION:
* Wait for a specific child and set the provided `last_exit_status`.

 BEHAVIOR:
* Calls `waitpid` for `pid`, updates `*last_exit_status` with either
* the child's exit status or a signal-derived code (128 + signum). Prints
* messages for SIGQUIT and SIGINT to mimic shell behavior.

 PARAMETERS:
* int *last_exit_status: Pointer to store the resolved exit status.
* pid_t pid: PID of the child to wait for.

 RETURN:
* `0` on success, `-1` if `waitpid` fails.
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:01:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/03 21:36:28 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/* ONLY global variable allowed - stores signal number for checking in loop */
volatile sig_atomic_t	g_signum = 0;

/*
** Interactive mode signal handler for SIGINT (Ctrl+C)
** Only sets the global flag - actual handling happens via rl_event_hook
** Uses only async-signal-safe functions (write)
*/
static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	ft_putstr_fd("\n", STDOUT_FILENO);
}

/*
** Set all signals to default behavior (for child processes)
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

/*
** Set signals to be ignored (for parent during child execution)
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

/*
** Initialize signals for interactive mode (waiting for input)
** SIGINT: Custom handler (new prompt)
** SIGQUIT: Ignored
** SIGTERM: Ignored
** SIGPIPE: Ignored
*/
int	set_signals_interactive(void)
{
	struct sigaction	sa;

	g_signum = 0;
	ft_bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	sa.sa_handler = interactive_sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	return (0);
}

/*
** Handle child process termination and set exit status
** Returns: 0 on success, -1 on error
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
			ft_putstr_fd("Core dumped\n", STDOUT_FILENO);
		else if (WTERMSIG(status) == SIGINT)
			ft_putstr_fd("\n", STDOUT_FILENO);
	}
	else if (WIFEXITED(status))
		*last_exit_status = WEXITSTATUS(status);
	else
		*last_exit_status = EXIT_FAILURE;
	return (0);
}

/*
** Event hook for readline - called periodically during input
** Checks for pending signals and handles them safely
** This is safe to call readline functions because it runs outside the signal handler context
** Note: Multiple rapid SIGINTs may coalesce into a single handling, which is acceptable
** Returns: 0 to continue readline operation
*/
int	readline_event_hook(void)
{
	if (g_signum == SIGINT)
	{
		g_signum = 0;
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	return (0);
}

/*
** Check and handle signal received during readline
** Call this AFTER readline returns in main loop
** Returns: 1 if signal was handled, 0 otherwise
*/
int	check_signal_received(t_shell *shell)
{
	if (g_signum == SIGINT)
	{
		shell->last_exit = 130;
		g_signum = 0;
		return (1);
	}
	return (0);
}
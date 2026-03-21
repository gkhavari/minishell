/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:01:22 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:27:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signum = 0;

static void	interactive_sigint_handler(int signum)
{
	(void)signum;
	g_signum = SIGINT;
	ft_putstr_fd("\n", STDOUT_FILENO);
}

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

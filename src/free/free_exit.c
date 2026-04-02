/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 14:00:00 by thanh-ng        #+#    #+#             */
/*   Updated: 2026/04/01 14:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Teardown then exit: free heap (incl. commands → heredoc fds), close 0/1/2,
** exit. Closing std fds is redundant with kernel close-on-exit but helps
** Valgrind --track-fds and makes intent explicit; it does not replace closing
** pipe/redir fds elsewhere — those must be closed before this path.
**
** clean_exit_before_readline: fork children + early init fatal — no rl_clear.
** clean_exit: main shell after readline (exit builtin); clears history first.
*/
void	clean_exit_before_readline(t_shell *shell, int exit_status)
{
	free_all(shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}

void	clean_exit(t_shell *shell, int exit_status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}

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

/**
 * free_all, close std fds, exit. Child/early-fatal path (no readline history).
 */
void	exit_norl(t_shell *shell, int exit_status)
{
	free_all(shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}

/**
 * Like exit_norl but rl_clear_history first (normal exit after readline).
 */
void	clean_exit(t_shell *shell, int exit_status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}

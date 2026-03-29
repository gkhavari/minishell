/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:34:11 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 20:55:10 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Cleanly terminate a child process executing a shell command.

 BEHAVIOR:
* Frees runtime resources associated with the shell in the child
* process and calls `_exit` with the provided status to avoid
* running parent cleanup logic.

 PARAMETERS:
* t_shell *shell: runtime to free in the child before exiting.
* int status: exit status to pass to `_exit`.

 RETURN:
* This function does not return; it calls `_exit`.
*/
void	exit_child(t_shell *shell, int status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(status);
}

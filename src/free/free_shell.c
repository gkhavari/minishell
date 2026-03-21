/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 18:16:13 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Free a NULL-terminated environment array and all its entries.

 BEHAVIOR:
* Frees each string in `envp` and then frees the array itself.

 PARAMETERS:
* char **envp: Environment array to free; may be NULL.
*/
static void	free_envp(char **envp)
{
	int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		envp[i] = NULL;
		i++;
	}
	free(envp);
}

/**
 DESCRIPTION:
* Reset transient shell state while preserving the persistent environment.

 BEHAVIOR:
* Frees tokens, commands and the input buffer but leaves `envp`, `user`,
* and other persistent fields intact so the shell can continue. This avoids
* leaking transient memory between commands and defends against using stale
* pointers in subsequent iterations.

 PARAMETERS:
* t_shell *shell: Shell runtime to reset; if NULL nothing is done.
*/
void	reset_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
		free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (shell->commands)
		free_commands(shell->commands);
	shell->commands = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
}

/**
 DESCRIPTION:
* Free the entire shell runtime state and associated allocations.

 BEHAVIOR:
* Frees tokens, commands, environment array, user, cwd and input fields and
* nulls the corresponding pointers. Intended for final cleanup or fatal
* error paths; prevents resource leaks across process lifetime.

 PARAMETERS:
* t_shell *shell: Shell runtime to free; if NULL nothing is done.
*/
void	free_all(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
		free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (shell->commands)
		free_commands(shell->commands);
	shell->commands = NULL;
	if (shell->envp)
		free_envp(shell->envp);
	shell->envp = NULL;
	if (shell->user)
		free(shell->user);
	shell->user = NULL;
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
}

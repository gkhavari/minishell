/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 22:50:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/02/24 22:20:53 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/** Free partial lexer state after MSH_OOM (word buffer + tokens + line). */
void	msh_lex_abort(t_shell *shell, char **word)
{
	if (word && *word)
		free(*word);
	if (word)
		*word = NULL;
	if (shell->tokens)
		free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
	shell->last_exit = FAILURE;
	shell->oom = 0;
}

/** Free tokens, commands, and input for the next prompt (keep env/user/cwd). */
void	reset_shell(t_shell *shell)
{
	if (!shell)
		return ;
	shell->oom = 0;
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

/** Full teardown: env, user, cwd, tokens, commands, input. */
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

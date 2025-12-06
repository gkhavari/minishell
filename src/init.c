/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:55:26 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/25 21:55:28 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 * Creating the prompt: "USER_NAME@minishell:CWD$ "
**/
char	*build_prompt(t_shell *shell)
{
	char		*prompt;
	size_t		total_len;
	const char	*user;
	const char	*cwd;

	if (shell->user != NULL)
		user = shell->user;
	else
		user = PROMPT_DEFAULT_USER;
	if (shell->cwd != NULL)
		cwd = shell->cwd;
	else
		cwd = PROMPT_DEFAULT_CWD;
	total_len = ft_strlen(user) + ft_strlen(cwd)
		+ ft_strlen(PROMPT_PREFIX) + ft_strlen(PROMPT_SUFFIX);
	prompt = ft_calloc(total_len + 1, sizeof(char));
	if (!prompt)
		return (perror("Error: malloc"), NULL);
	ft_strcat(prompt, user);
	ft_strcat(prompt, PROMPT_PREFIX);
	ft_strcat(prompt, cwd);
	ft_strcat(prompt, PROMPT_SUFFIX);
	return (prompt);
}

/*Initialize shell struct*/
void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = envp;
	shell->user = ft_strdup(get_env_value(envp, "USER"));
	shell->cwd = getcwd(NULL, 0);
	if (!shell->cwd)
		return (perror(strerror(errno)));
	shell->last_exit = 0;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
}

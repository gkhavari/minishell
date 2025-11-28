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
	const char	*username;
	const char	*current_directory;

	if (shell->user != NULL)
		username = shell->user;
	else
		username = PROMPT_DEFAULT_USER;
	if (shell->cwd != NULL)
		current_directory = shell->cwd;
	else
		current_directory = PROMPT_DEFAULT_CWD;
	total_len = ft_strlen(username) + ft_strlen(current_directory)
		+ ft_strlen(PROMPT_PREFIX) + ft_strlen(PROMPT_SUFFIX);
	prompt = ft_calloc(total_len + 1, sizeof(char));
	if (!prompt)
		return (perror("Error: malloc"), NULL);
	ft_strcat(prompt, username);
	ft_strcat(prompt, PROMPT_PREFIX);
	ft_strcat(prompt, current_directory);
	ft_strcat(prompt, PROMPT_SUFFIX);
	return (prompt);
}

/*Helper to get environment variable value*/
char	*get_env_value(char **envp, const char *env_key)
{
	int		index;
	size_t	key_len;

	index = 0;
	key_len = ft_strlen(env_key);
	while (envp[index])
	{
		if (ft_strncmp(envp[index], env_key, key_len) == 0
			&& envp[index][key_len] == '=')
			return (envp[index] + key_len + 1);
		index++;
	}
	return (NULL);
}

/*Initialize shell struct*/
void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = envp;
	shell->user = get_env_value(envp, "USER");
	shell->cwd = getcwd(NULL, 0);
	if (!shell->cwd)
		return (perror(strerror(errno)));
	shell->last_exit = 0;
}

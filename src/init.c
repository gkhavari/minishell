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
	char	*prompt;
	size_t	len_user;
	size_t	len_cwd;
	size_t	total_len;

	if (shell->user != NULL)
		len_user = strlen(shell->user);
	else
		len_user = 4;
	len_cwd = strlen(shell->cwd);
	total_len = len_user + 11 + len_cwd;
	prompt = ft_calloc(total_len, sizeof(char));
	if (!prompt)
	{
		perror("Error: malloc");
		return (NULL);
	}
	if (shell->user)
		ft_strcat(prompt, shell->user);
	else
		ft_strcat(prompt, "user");
	ft_strcat(prompt, "@minishell:");
	ft_strcat(prompt, shell->cwd);
	ft_strcat(prompt, "$ ");
	return (prompt);
}

/*Helper to get environment variables*/
char	*get_env(char **envp, const char *key)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(key);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

#include <errno.h>
/*Initialize shell struct*/
void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = envp;
	shell->user = get_env(envp, "USER");
//	shell->cwd = get_env(envp, "PWD");
	shell->cwd = getcwd(NULL, 0);
	if (!shell->cwd)
		return (perror(strerror(errno)));
	shell->last_exit = 0;
}

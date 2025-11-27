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

#include "../includes/minishell.h"

char	*build_prompt(t_shell *shell)
{
	char	*prompt;
	char	cwd[PATH_MAX];
	size_t	len_user;
	size_t	len_cwd;
	size_t	total_len;

	// Get current working directory
	if (!getcwd(cwd, sizeof(cwd)))
		perror("Error: getcwd");
	// Calculate lengths
	if (shell->user != NULL)
		len_user = strlen(shell->user);
	else
		len_user = 4; // length of "user"
	len_cwd = strlen(cwd);
	total_len = len_user + 11 + len_cwd;
	// 11 = "@minishell:" + "$ " + null terminator
	// Allocate memory
	prompt = malloc(total_len);
	if (!prompt)
	{
		perror("Error: malloc");
		return (NULL);
	}
	// Build string manually
	prompt[0] = '\0';
	if (shell->user)
		ft_strcat(prompt, shell->user);
	else
		ft_strcat(prompt, "user");
	ft_strcat(prompt, "@minishell:");
	ft_strcat(prompt, cwd);
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

/*Initialize shell struct*/
void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = envp;
	shell->user = get_env(envp, "USER");
	getcwd(shell->cwd, sizeof(shell->cwd));
	shell->last_exit = 0;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 23:43:00 by minishell         #+#    #+#             */
/*   Updated: 2026/03/31 23:43:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_env_value(char **envp, const char *key)
{
	size_t	key_len;
	int		i;

	if (!envp || !key)
		return (NULL);
	key_len = 0;
	while (key[key_len])
		key_len++;
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, key_len) == 0
			&& envp[i][key_len] == '=')
			return (envp[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

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
		return (NULL);
	ft_strcat(prompt, user);
	ft_strcat(prompt, PROMPT_PREFIX);
	ft_strcat(prompt, cwd);
	ft_strcat(prompt, PROMPT_SUFFIX);
	return (prompt);
}

void	move_last_env_to_front(t_shell *shell)
{
	int		count;
	char	*tmp;

	count = 0;
	while (shell->envp[count])
		count++;
	tmp = shell->envp[count - 1];
	while (count > 1)
	{
		shell->envp[count - 1] = shell->envp[count - 2];
		count--;
	}
	shell->envp[0] = tmp;
}

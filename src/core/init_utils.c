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

#include "minishell.h"

static void	init_fatal_errno(t_shell *shell, const char *detail)
{
	if (detail != NULL)
		ft_dprintf(STDERR_FILENO,
			SH_NAME ": %s: %s\n", detail, strerror(errno));
	else
		ft_dprintf(STDERR_FILENO, SH_NAME ": %s\n", strerror(errno));
	clean_exit_before_readline(shell, FAILURE);
}

/** Dup envp, USER, cwd (fallback "/"); never returns on allocation failure. */
void	init_shell_identity(t_shell *shell, char **envp)
{
	char	*user;

	shell->envp = ft_arrdup(envp);
	if (!shell->envp)
		init_fatal_errno(shell, "failed to duplicate environment");
	user = get_env_value(shell->envp, "USER");
	if (!user)
		shell->user = NULL;
	else
	{
		shell->user = ft_strdup(user);
		if (!shell->user)
			init_fatal_errno(shell, NULL);
	}
	shell->cwd = getcwd(NULL, 0);
	if (shell->cwd == NULL)
	{
		shell->cwd = ft_strdup("/");
		if (shell->cwd == NULL)
			init_fatal_errno(shell, NULL);
	}
}

/** Value after KEY= in envp, or NULL. */
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

/** user + cwd prompt string; caller frees (readline). */
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

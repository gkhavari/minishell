/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:55:26 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 13:54:13 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** build_prompt - Create the shell prompt string
** Format: "USER@minishell:CWD$ "
** Falls back to defaults if user or cwd are not set.
** Returns: newly allocated prompt string, or NULL on malloc failure.
*/
char	*get_env_value(char **envp, const char *key)
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
	prompt = msh_calloc(shell, total_len + 1, sizeof(char));
	ft_strcat(prompt, user);
	ft_strcat(prompt, PROMPT_PREFIX);
	ft_strcat(prompt, cwd);
	ft_strcat(prompt, PROMPT_SUFFIX);
	return (prompt);
}

static void	update_shlvl(t_shell *shell)
{
	char	*shlvl_val;
	char	*num_str;
	char	*entry;
	int		shlvl;
	int		idx;

	shlvl_val = get_env_value(shell->envp, "SHLVL");
	if (shlvl_val)
		shlvl = ft_atoi(shlvl_val) + 1;
	else
		shlvl = 1;
	num_str = ft_itoa(shlvl);
	if (!num_str)
		return ;
	entry = ft_strjoin("SHLVL=", num_str);
	free(num_str);
	if (!entry)
		return ;
	idx = find_export_key_index(shell, "SHLVL", 5);
	if (idx >= 0)
		(free(shell->envp[idx]), (shell->envp[idx] = entry));
	else
		(append_export_env(shell, entry), free(entry));
}

static void	init_runtime_fields(t_shell *shell)
{
	shell->last_exit = 0;
	shell->barrier_write_fd = -1;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
	shell->word_quoted = 0;
	shell->heredoc_mode = 0;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
}

void	init_shell(t_shell *shell, char **envp)
{
	char	*user;

	shell->envp = ft_arrdup(envp);
	if (!shell->envp)
	{
		perror("minishell: failed to duplicate environment");
		exit(1);
	}
	user = get_env_value(shell->envp, "USER");
	if (user)
		shell->user = ft_strdup(user);
	else
		shell->user = NULL;
	shell->cwd = getcwd(NULL, 0);
	if (!shell->cwd)
		shell->cwd = ft_strdup("/");
	init_runtime_fields(shell);
	if (isatty(STDIN_FILENO))
		update_shlvl(shell);
}

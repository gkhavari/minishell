/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:55:26 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:20:30 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Retrieve the value part of an environment variable from `envp`.

 BEHAVIOR:
* Scans the NULL-terminated `envp` array for an entry beginning with
* `key=` and returns a pointer to the value substring within the
* existing `envp` entry. Does not allocate a new string.

 PARAMETERS:
* char **envp: NULL-terminated environment array.
* const char *key: Variable name to search for (without `=`).

 RETURN:
* Pointer to the value within `envp` if found, otherwise NULL.
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

/**
 DESCRIPTION:
* Build and return a newly allocated prompt string.

 BEHAVIOR:
* Concatenates user, prefix, cwd and suffix into a single allocated
* string (e.g. "user@minishell:/cwd$ "). Falls back to defaults when
* `shell->user` or `shell->cwd` are NULL.

 PARAMETERS:
* t_shell *shell: Shell runtime providing `user` and `cwd`.

 RETURN:
* Allocated prompt string, or NULL on allocation failure.
*/
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

/**
 DESCRIPTION:
* Update the `SHLVL` environment variable for this shell instance.

 BEHAVIOR:
* Increments `SHLVL` if present, otherwise sets it to `1`. Updates the
* exported environment array, replacing an existing entry or appending
* a new one.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `envp` will be modified.

 RETURN:
* None.
*/
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

void	normalize_child_shlvl(t_shell *shell)
{
	char	*shlvl_val;
	char	*num_str;
	char	*entry;
	int		lvl;
	int		idx;

	shlvl_val = get_env_value(shell->envp, "SHLVL");
	if (!shlvl_val)
		return ;
	lvl = ft_atoi(shlvl_val);
	if (lvl > 0)
		lvl--;
	num_str = ft_itoa(lvl);
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

/**
 DESCRIPTION:
* Initialize the shell runtime structure from the provided environment.

 BEHAVIOR:
* Duplicates `envp`, sets `user`, `cwd`, and initial runtime fields,
* records whether `PATH` was present, and increments `SHLVL`.
* Exits the process on fatal allocation failure.

 PARAMETERS:
* t_shell *shell: Uninitialized shell structure to populate.
* char **envp: Process environment to duplicate.

 RETURN:
* None.
*/
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
	shell->last_exit = 0;
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->input = NULL;
	shell->had_path = (get_env_value(shell->envp, "PATH") != NULL);
	update_shlvl(shell);
}

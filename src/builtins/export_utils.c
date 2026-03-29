/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:40 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:21:07 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Validate an identifier name for `export`.

 BEHAVIOR:
* Ensures the name is non-empty, starts with a letter or `_` and
* contains only alphanumeric characters or underscores afterwards.

 PARAMETERS:
* char *name: Identifier to validate.

 RETURN:
* `1` if valid, `0` otherwise.
*/
int	is_valid_export_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/**
 DESCRIPTION:
* Find the index of an environment entry matching `key`.

 BEHAVIOR:
* Iterates `shell->envp` comparing the first `key_len` characters and
* verifies that the following character is `=` or the end of string.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `envp`.
* char *key: The key to search for (not terminated by `=`).
* int key_len: Length of the key to compare.

 RETURN:
* Index of the matching entry, or `-1` if not found.
*/
int	find_export_key_index(t_shell *shell, char *key, int key_len)
{
	int		i;
	int		env_key_len;
	char	*eq;

	i = 0;
	while (shell->envp[i])
	{
		eq = ft_strchr(shell->envp[i], '=');
		if (eq)
			env_key_len = (int)(eq - shell->envp[i]);
		else
			env_key_len = ft_strlen(shell->envp[i]);
		if (env_key_len == key_len
			&& ft_strncmp(shell->envp[i], key, key_len) == 0)
			return (i);
		i++;
	}
	return (-1);
}

/**
 DESCRIPTION:
* Append a new environment entry to the shell's `envp` array.

 BEHAVIOR:
* Allocates a new array one element larger, copies existing entries,
* duplicates `entry` into the new slot and replaces `shell->envp`.

 PARAMETERS:
* t_shell *shell: Shell runtime whose `envp` will be extended.
* char *entry: String to append (format `key` or `key=value`).

 RETURN:
* `0` on success, non-zero on allocation failure.
*/
int	append_export_env(t_shell *shell, char *entry)
{
	char	**new_envp;
	int		count;
	int		i;

	count = 0;
	while (shell->envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (1);
	i = -1;
	while (++i < count)
		new_envp[i] = shell->envp[i];
	new_envp[count] = ft_strdup(entry);
	if (!new_envp[count])
		return (free(new_envp), 1);
	new_envp[count + 1] = NULL;
	free(shell->envp);
	shell->envp = new_envp;
	return (0);
}

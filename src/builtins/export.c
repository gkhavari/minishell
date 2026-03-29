/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:45 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 22:21:13 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Replace an existing environment entry or append a new one.

 BEHAVIOR:
* If the key exists in `shell->envp` replaces it with `arg` otherwise
* appends `arg` to the environment array.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `envp`.
* char *arg: Full `key` or `key=value` string to set.
* char *key: Key string used for lookup.

 RETURN:
* `0` on success, non-zero on failure.
*/
static int	replace_or_append(t_shell *shell, char *arg, char *key)
{
	int	idx;

	idx = find_export_key_index(shell, key, ft_strlen(key));
	if (idx >= 0)
	{
		free(shell->envp[idx]);
		shell->envp[idx] = ft_strdup(arg);
		return (shell->envp[idx] == NULL);
	}
	return (append_export_env(shell, arg));
}

/**
 DESCRIPTION:
* Handle an `export` argument that contains no '='.

 BEHAVIOR:
* Validates the identifier and appends it to env if not present.
* Prints an error when the name is invalid.

 PARAMETERS:
* t_shell *shell: Shell runtime to modify.
* char *arg: Identifier string (no `=`).

 RETURN:
* `0` on success, non-zero on error.
*/
static int	export_no_value(t_shell *shell, char *arg)
{
	if (!is_valid_export_name(arg))
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (1);
	}
	if (find_export_key_index(shell, arg, ft_strlen(arg)) < 0)
		return (append_export_env(shell, arg));
	return (0);
}

/**
 DESCRIPTION:
* Handle append-mode `key+=value` during export.

 BEHAVIOR:
* Reads the existing value (if any), concatenates the new suffix and
* sets the combined string as the new environment entry.
* Frees intermediate allocations on error.

 PARAMETERS:
* t_shell *shell: Shell runtime to update.
* char *key: Key portion (without `=`) that will be reused.
* char *eq: Pointer inside original `arg` to the '=' character.

 RETURN:
* `0` on success, non-zero on failure.
*/
static int	handle_append(t_shell *shell, char *key, char *eq)
{
	char	*old_val;
	char	*tmp;
	char	*new_entry;
	int		ret;

	old_val = get_env_value(shell->envp, key);
	if (old_val)
		tmp = ft_strjoin(old_val, eq + 1);
	else
		tmp = ft_strdup(eq + 1);
	if (!tmp)
		return (free(key), 1);
	new_entry = ft_strjoin(key, "=");
	free(key);
	if (!new_entry)
		return (free(tmp), 1);
	key = ft_strjoin(new_entry, tmp);
	free(new_entry);
	free(tmp);
	if (!key)
		return (1);
	ret = replace_or_append(shell, key, key);
	return (free(key), ret);
}

/**
 DESCRIPTION:
* Parse and set an environment variable from an export argument.

 BEHAVIOR:
* If the argument contains no '=' validates and handles it as
* a name-only export. If it contains `=` handles normal set or
* append (`+=`) semantics and reports invalid identifiers.

 PARAMETERS:
* t_shell *shell: Shell runtime to update.
* char *arg: Argument string passed to `export`.

 RETURN:
* `0` on success, non-zero on error.
*/
static int	set_env_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;
	int		append_mode;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (export_no_value(shell, arg));
	append_mode = (eq > arg && *(eq - 1) == '+');
	if (append_mode)
		key = ft_substr(arg, 0, eq - arg - 1);
	else
		key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (1);
	if (!is_valid_export_name(key))
	{
		ft_putstr_fd("export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (free(key), 1);
	}
	if (!append_mode)
		return (replace_or_append(shell, arg, key), free(key), 0);
	return (handle_append(shell, key, eq));
}

/**
 DESCRIPTION:
* `export` builtin: set or list environment variables.

 BEHAVIOR:
* With no arguments prints the environment in sorted `declare -x` form.
* Otherwise processes each argument, handling invalid options and
* invalid identifiers, performing set or append semantics as required.

 PARAMETERS:
* char **args: Argument vector for `export`.
* t_shell *shell: Shell runtime containing `envp` to modify.

 RETURN:
* Aggregate exit status: `0` on success, non-zero on errors.
*/
int	builtin_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
		return (print_sorted_env(shell));
	i = 1;
	ret = 0;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			ft_putstr_fd("export: ", 2);
			ft_putstr_fd(args[i], 2);
			ft_putendl_fd(": invalid option", 2);
			ret = 2;
		}
		else if (set_env_var(shell, args[i]))
			ret = 1;
		i++;
	}
	return (ret);
}

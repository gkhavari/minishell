#include "minishell.h"

/*
** replace_or_append - Replace existing or append new env var
** @shell: shell state
** @arg: KEY=value string
** @key: extracted key
** Return: 0 on success, 1 on failure
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

/*
** set_env_var - Parse and set environment variable
** @shell: shell state
** @arg: KEY=value string to set
** Return: 0 on success or no '=', 1 on error
*/
static int	set_env_var(t_shell *shell, char *arg)
{
	char	*eq;
	char	*key;
	int		ret;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (0);
	key = ft_substr(arg, 0, eq - arg);
	if (!key)
		return (1);
	if (!is_valid_export_name(key))
	{
		free(key);
		ft_putstr_fd("minishell: export: '", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (1);
	}
	ret = replace_or_append(shell, arg, key);
	free(key);
	return (ret);
}

/*
** builtin_export - Set environment variables
** @args: command arguments (args[1..n] = KEY=value)
** @shell: shell state
** Return: 0 on success, 1 if any arg failed
*/
int	builtin_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
		return (builtin_env(args, shell));
	i = 1;
	ret = 0;
	while (args[i])
	{
		if (set_env_var(shell, args[i]))
			ret = 1;
		i++;
	}
	return (ret);
}

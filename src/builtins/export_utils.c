#include "minishell.h"

/*
** is_valid_name - Check if name is valid env identifier
** @name: variable name to check
** Return: 1 if valid, 0 otherwise
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

/*
** find_key_index - Find index of env var by key
** @shell: shell state
** @key: variable name
** @key_len: length of key
** Return: index if found, -1 otherwise
*/
int	find_export_key_index(t_shell *shell, char *key, int key_len)
{
	int	i;

	i = 0;
	while (shell->envp[i])
	{
		if (ft_strncmp(shell->envp[i], key, key_len) == 0
			&& shell->envp[i][key_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

/*
** append_env - Add new entry to envp array
** @shell: shell state
** @entry: KEY=value string to add
** Return: 0 on success, 1 on malloc failure
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

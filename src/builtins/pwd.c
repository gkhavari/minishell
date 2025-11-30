#include "minishell.h"

/*
** builtin_pwd - Print current working directory
** @args: unused
** @shell: shell state for fallback cwd
** Return: 0 on success, 1 on failure
*/
int	builtin_pwd(char **args, t_shell *shell)
{
	char	*cwd;

	(void)args;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		if (shell->cwd)
			cwd = ft_strdup(shell->cwd);
		if (!cwd)
			return (1);
	}
	ft_putendl_fd(cwd, 1);
	free(cwd);
	return (0);
}

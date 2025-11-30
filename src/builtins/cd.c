#include "minishell.h"

/*
** get_cd_target - Get target directory for cd
** @args: command arguments
** @shell: shell state for HOME lookup
** Return: target path or NULL if HOME not set
*/
static char	*get_cd_target(char **args, t_shell *shell)
{
	char	*target;

	if (!args[1])
	{
		target = get_env(shell->envp, "HOME");
		if (!target)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (NULL);
		}
		return (target);
	}
	return (args[1]);
}

/*
** update_shell_cwd - Update shell's current working directory
** @shell: shell state to update
** Return: 0 on success, 1 on failure
*/
static int	update_shell_cwd(t_shell *shell)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (1);
	if (shell->cwd)
		free(shell->cwd);
	shell->cwd = cwd;
	return (0);
}

/*
** builtin_cd - Change current directory
** @args: command arguments (args[1] = path or empty for HOME)
** @shell: shell state
** Return: 0 on success, 1 on failure
*/
int	builtin_cd(char **args, t_shell *shell)
{
	char	*target;

	target = get_cd_target(args, shell);
	if (!target)
		return (1);
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(target, 2);
		ft_putendl_fd(": No such file or directory", 2);
		return (1);
	}
	return (update_shell_cwd(shell));
}

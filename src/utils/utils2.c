#include "minishell.h"

char *msh_readline(t_shell *shell, char *prompt)
{
	char	*res;

	res = readline(prompt);
	if (res == NULL)
		clean_exit(shell, EXIT_FAILURE);
	return (res);
}

void	clean_exit(t_shell *shell, int exit_status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}

char	*msh_strdup(t_shell *shell, char *s)
{
	char	*res;

	res = NULL;
	res = ft_strdup(s);
	if (res == NULL)
		clean_exit(shell, EXIT_FAILURE);
	return (res);
}

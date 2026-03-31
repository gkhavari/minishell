#include "minishell.h"

char *msh_readline(t_shell *shell, char *prompt)
{
	char	*res;

	res = readline(prompt);
	if (res == NULL)
	{
		free_all(shell);
		exit(EXIT_FAILURE);
	}
	return (res);
}

void	free_early(t_shell *shell, int exit_status)
{
	free_all(shell);
	rl_clear_history();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(exit_status);
}
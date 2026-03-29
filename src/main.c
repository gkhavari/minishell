/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 19:56:09 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** read_input - Display prompt and read one line of user input.
** Returns 1 on success, 0 on EOF (Ctrl+D), -1 on signal.
** When stdin is not a TTY (e.g. tester) uses read_line_stdin.
*/
static char	*read_line_stdin(t_shell *shell)
{
	char	*line;
	char	c;
	int		ret;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (ft_strlen(line) == 0)
				return (free(line), NULL);
			return (line);
		}
		if (c == '\n')
			return (line);
		append_char(shell, &line, c);
	}
}

static int	read_input(t_shell *shell)
{
	char	*prompt;

	if (!isatty(STDIN_FILENO))
		shell->input = read_line_stdin(shell);
	else
	{
		prompt = build_prompt(shell);
		if (!prompt)
			return (0);
		shell->input = readline(prompt);
		free(prompt);
	}
	if (!shell->input)
	{
		if (isatty(STDIN_FILENO))
			ft_putstr_fd("exit\n", STDOUT_FILENO);
		return (0);
	}
	if (check_signal_received(shell))
		return (free(shell->input), shell->input = NULL, -1);
	return (1);
}

/*
** shell_loop - Main REPL loop following architecture doc
** 1. Check signals  2. Build prompt  3. Read input
** 4. Check signals again  5. Process (lex/parse/expand/execute)
** 6. Cleanup
*/
static void	shell_loop(t_shell *shell)
{
	int	status;
	int	syntax_err;

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		if (status == 0)
			break ;
		if (status == -1)
			continue ;
		syntax_err = 0;
			if (shell->input[0])
				process_input(shell);
		if (!shell->commands && shell->last_exit == 2)
			syntax_err = 1;
		reset_shell(shell);
		if (!isatty(STDIN_FILENO) && syntax_err)
			break ;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, envp);
	set_signals_interactive();
	if (isatty(STDIN_FILENO))
		rl_event_hook = readline_event_hook;
	shell_loop(&shell);
	rl_clear_history();
	free_all(&shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return (shell.last_exit);
}

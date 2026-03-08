/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** process_input - Tokenize, parse, process heredocs, execute
** Runs the full pipeline for one line of user input.
** Returns early at any stage that fails (syntax error, signal, etc).
*/
static void	process_input(t_shell *shell)
{
	tokenize_input(shell);
	parse_input(shell);
	if (!shell->commands)
		return ;
	if (process_heredocs(shell))
	{
		shell->last_exit = 130;
		return ;
	}
	shell->last_exit = execute_commands(shell);
}

/*
** read_input - Display prompt and read one line of user input
** Returns 1 on success, 0 on EOF (Ctrl+D), -1 on signal.
*/
/*
** silent_readline - Call readline with stdout suppressed (non-interactive)
** Redirects stdout to /dev/null during readline to suppress echo.
*/
static char	*silent_readline(void)
{
	int		saved;
	int		devnull;
	char	*line;

	saved = dup(STDOUT_FILENO);
	devnull = open("/dev/null", O_WRONLY);
	if (devnull >= 0)
	{
		dup2(devnull, STDOUT_FILENO);
		close(devnull);
	}
	line = readline("");
	dup2(saved, STDOUT_FILENO);
	close(saved);
	return (line);
}

static int	read_input(t_shell *shell)
{
	char	*prompt;

	if (!isatty(STDIN_FILENO))
	{
		shell->input = silent_readline();
	}
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

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		if (status == 0)
			break ;
		if (status == -1)
			continue ;
		if (shell->input[0])
			process_input(shell);
		free_commands(shell->commands);
		shell->commands = NULL;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		free(shell->input);
		shell->input = NULL;
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
	return (shell.last_exit);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 15:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Non-TTY: one line via ft_read_stdin_line (no shell->oom on OOM). */
static int	read_line_stdin(t_shell *shell, char **out)
{
	return (ft_read_stdin_line(shell, out, 0));
}

/**
 * TTY: readline; else read_line_stdin.
 * Returns READ_LINE, READ_EOF, READ_SIG, or OOM.
 */
static int	read_input(t_shell *shell)
{
	char	*prompt;
	int		tty;

	tty = isatty(STDIN_FILENO);
	if (tty != 1)
		return (read_line_stdin(shell, &shell->input));
	prompt = build_prompt(shell);
	if (!prompt)
		return (OOM);
	shell->input = readline(prompt);
	free(prompt);
	if (!shell->input)
	{
		ft_printf("exit\n");
		return (READ_EOF);
	}
	if (check_signal_received(shell))
		return (free(shell->input), shell->input = NULL, READ_SIG);
	return (READ_LINE);
}

/** One REPL iteration after a successful read_input. Returns 1 to exit loop. */
static int	repl_after_read(t_shell *shell)
{
	int	syntax_err;

	syntax_err = 0;
	if (shell->input[0])
		process_input(shell);
	if (!shell->commands && shell->last_exit == EXIT_SYNTAX_ERROR)
		syntax_err = 1;
	reset_shell(shell);
	if (isatty(STDIN_FILENO) != 1 && syntax_err)
		return (1);
	return (0);
}

/** REPL: read line, process_input, reset state on syntax error. */
static void	shell_loop(t_shell *shell)
{
	int	status;

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		if (status == READ_EOF)
			break ;
		if (status == READ_SIG)
			continue ;
		if (status == OOM)
		{
			shell->oom = 1;
			shell->last_exit = FAILURE;
			reset_shell(shell);
			continue ;
		}
		if (repl_after_read(shell))
			break ;
	}
}

/**
 * Program entry: init shell/env, interactive signals, REPL,
 * exit with last status.
 */
int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, envp);
	set_signals_interactive();
	if (isatty(STDIN_FILENO) == 1)
		rl_event_hook = readline_event_hook;
	shell_loop(&shell);
	rl_clear_history();
	free_all(&shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return (shell.last_exit);
}

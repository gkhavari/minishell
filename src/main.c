/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/30 20:03:25 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* read_input status — not $? / exit codes */
# define MSH_READ_LINE 1
# define MSH_READ_EOF 0
# define MSH_READ_SIG -1

/**
 * Non-TTY: one line from stdin byte-by-byte into *out.
 * On EOF with no bytes, *out is NULL. On OOM after partial read, *out is NULL.
 * Returns MSH_READ_LINE, MSH_READ_EOF, or MSH_OOM.
 */
static int	read_line_stdin(t_shell *shell, char **out)
{
	char	c;
	int		ret;

	*out = ft_strdup("");
	if (!*out)
		return (MSH_OOM);
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0)
		{
			if (ft_strlen(*out) == 0)
				return (free(*out), *out = NULL, MSH_READ_EOF);
			return (MSH_READ_LINE);
		}
		if (c == '\n')
			return (MSH_READ_LINE);
		if (append_char(shell, out, c) == MSH_OOM)
			return (MSH_OOM);
	}
}

/**
 * TTY: readline; else read_line_stdin.
 * Returns MSH_READ_LINE, MSH_READ_EOF, MSH_READ_SIG, or MSH_OOM.
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
		return (MSH_OOM);
	shell->input = readline(prompt);
	free(prompt);
	if (!shell->input)
	{
		ft_printf("exit\n");
		return (MSH_READ_EOF);
	}
	if (check_signal_received(shell))
		return (free(shell->input), shell->input = NULL, MSH_READ_SIG);
	return (MSH_READ_LINE);
}

/** REPL: read line, process_input, reset state on syntax error. */
static void	shell_loop(t_shell *shell)
{
	int	status;
	int	syntax_err;

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		if (status == MSH_READ_EOF)
			break ;
		if (status == MSH_READ_SIG)
			continue ;
		if (status == MSH_OOM)
		{
			shell->last_exit = FAILURE;
			continue ;
		}
		syntax_err = 0;
		if (shell->input[0])
			process_input(shell);
		if (!shell->commands && shell->last_exit == EXIT_SYNTAX_ERROR)
			syntax_err = 1;
		reset_shell(shell);
		if (isatty(STDIN_FILENO) != 1 && syntax_err)
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

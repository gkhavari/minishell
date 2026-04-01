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

/** Non-TTY: read one line from stdin byte-by-byte into a growing buffer. */
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
			if (!line || ft_strlen(line) == 0)
				return (free(line), NULL);
			return (line);
		}
		if (c == '\n')
			return (line);
		if (append_char(shell, &line, c) == MSH_OOM)
		{
			shell->oom = 1;
			return (NULL);
		}
	}
}

/** TTY: readline; else read_line_stdin. Returns 1, 0 (EOF), or -1 (signal). */
static int	read_input(t_shell *shell)
{
	char	*prompt;
	int		tty;

	tty = isatty(STDIN_FILENO);
	if (tty != 1)
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
		if (tty == 1)
			ft_printf("exit\n");
		return (0);
	}
	if (check_signal_received(shell))
		return (free(shell->input), shell->input = NULL, -1);
	return (1);
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
		if (status == 0)
			break ;
		if (status == -1)
			continue ;
		if (status == READ_INPUT_OOM)
		{
			shell->last_exit = 1;
			continue ;
		}
		syntax_err = 0;
		if (shell->input[0])
			process_input(shell);
		if (!shell->commands && shell->last_exit == 2)
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

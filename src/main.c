/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 20:20:43 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Run the full processing pipeline for the current input line.

 BEHAVIOR:
* Tokenizes and parses `shell->input` into commands, processes any
* heredocs, and executes the resulting commands pipeline. On a
* heredoc error sets `shell->last_exit` to 130 and aborts execution
* for this input line.

 PARAMETERS:
* t_shell *shell: Shell runtime holding the input and parsed commands.

 RETURN:
* None.
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

/**
 DESCRIPTION:
* Read a line from stdin when not operating interactively.

 BEHAVIOR:
* Reads bytes until a newline or EOF, appending characters into a
* dynamically allocated buffer. Returns NULL on EOF when no data was
* read, otherwise returns the allocated line (without the newline).

 PARAMETERS:
* t_shell *shell: Shell runtime used by `append_char` helper.

 RETURN:
* Allocated line string on success, or NULL on EOF/error.
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

/**
 DESCRIPTION:
* Read one line of input and store it in `shell->input`.

 BEHAVIOR:
* If stdin is a TTY builds and displays a prompt then uses
* `readline`; otherwise reads from stdin directly. Handles EOF and
* signal conditions and normalizes return values for the caller.

 PARAMETERS:
* t_shell *shell: Shell runtime where `input` will be stored.

 RETURN:
* `1` on successful read, `0` on EOF, `-1` when interrupted by a signal.
*/
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
	if (check_signal_received(shell)
		&& (!shell->input || shell->input[0] == '\0'))
		return (free(shell->input), shell->input = NULL, -1);
	return (1);
}

/**
 DESCRIPTION:
* Main read–eval–print loop for the shell.

 BEHAVIOR:
* Repeatedly checks signals, reads input, processes the input
* (tokenize/parse/execute), and resets transient runtime state.
* Exits on EOF or on non-interactive syntax errors.

 PARAMETERS:
* t_shell *shell: Initialized shell runtime.

 RETURN:
* None.
*/
static void	shell_loop(t_shell *shell)
{
	int	status;
	int	syntax_err;

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		(void)shell;
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

/**
 DESCRIPTION:
* Program entry point and high-level lifecycle management.

 BEHAVIOR:
* Initializes the `t_shell` runtime from the environment, configures
* signal handlers for interactive use, installs the `readline` event
* hook when running on a TTY, runs the main REPL loop, and performs
* final cleanup (readline history + freeing runtime state) before
* returning the last command exit status to the caller.

 PARAMETERS:
* int argc: Argument count (unused).
* char **argv: Argument vector (unused).
* char **envp: Environment pointer array passed to the shell init.

 RETURN:
* Process exit code equivalent to `shell.last_exit`.
*/
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

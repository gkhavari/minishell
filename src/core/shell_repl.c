/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_repl.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Non-TTY: one line via ft_read_stdin_line (no shell->oom on OOM). */
static int	read_line_stdin(t_shell *shell, char **out)
{
	return (ft_read_stdin_line(shell, out, 0));
}

/** user + cwd prompt string; caller frees (readline). */
static char	*build_prompt(t_shell *shell)
{
	char		*prompt;
	size_t		total_len;
	const char	*user;
	const char	*cwd;

	if (shell->user != NULL)
		user = shell->user;
	else
		user = PM_USR;
	if (shell->cwd != NULL)
		cwd = shell->cwd;
	else
		cwd = PM_CWD;
	total_len = ft_strlen(user) + ft_strlen(cwd)
		+ ft_strlen(PM_PFX) + ft_strlen(PM_SFX);
	prompt = ft_calloc(total_len + 1, sizeof(char));
	if (!prompt)
		return (NULL);
	ft_strcat(prompt, user);
	ft_strcat(prompt, PM_PFX);
	ft_strcat(prompt, cwd);
	ft_strcat(prompt, PM_SFX);
	return (prompt);
}

/**
 * TTY: readline; else read_line_stdin.
 * Returns RL_LN, RL_EOF, RL_SIG, or OOM.
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
		return (RL_EOF);
	}
	if (check_signal_received(shell))
		return (free(shell->input), shell->input = NULL, RL_SIG);
	return (RL_LN);
}

/** One REPL iteration after read_input. Returns 1 to exit loop. */
static int	repl_after_read(t_shell *shell)
{
	int	syntax_err;

	syntax_err = 0;
	if (shell->input[0])
		process_input(shell);
	if (!shell->commands && shell->last_exit == XSYN)
		syntax_err = 1;
	reset_shell(shell);
	if (isatty(STDIN_FILENO) != 1 && syntax_err)
		return (1);
	return (0);
}

/** REPL: read line, process_input, reset on syntax error. */
void	shell_loop(t_shell *shell)
{
	int	status;

	while (1)
	{
		check_signal_received(shell);
		status = read_input(shell);
		if (status == RL_EOF)
			break ;
		if (status == RL_SIG)
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

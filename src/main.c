/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/25 21:11:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler(int signum)
{
	(void)signum;
	write(1, "\n", 1);
	rl_replace_line("", 0); // Clear current input
	rl_on_new_line();       // Move to new line
	rl_redisplay();         // Redisplay prompt
}

void	disable_ctrl_echo(void)
{
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL; // Disable printing ^C, ^D, etc.
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*input;
	char	*promt;

	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	(void) argc;
	(void) argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, envp);
	disable_ctrl_echo();
	while (1)
	{
		promt = build_prompt(&shell);
		if (!promt)
		{
			perror("Failed to build prompt");
			break ;
		}
		errno = 0;
		input = readline(promt);
		free(promt);
		if (!input)
		{
			if (errno)
				perror("readline failed");
			else
				ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (*input)
			add_history(input);
		free(input);
	}
	rl_clear_history();
	free(shell.cwd);
	return (0);
}

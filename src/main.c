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

void print_tokens(t_shell *shell)
{
	t_token *t = shell->tokens;

	while (t != NULL)
	{
		printf("%d\n", t->type);
		printf("%s\n", t->value);
		printf("\n");
		t = t->next;
	}
}

void	sigint_handler(int signum)
{
	(void)signum;
	write(1, "\n", 1);
	rl_replace_line("", 0);	// Clear current input
	rl_on_new_line();		// Move to new line
	rl_redisplay();			// Redisplay prompt
}

void	disable_ctrl_echo(void)
{
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*promt;

	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);
	(void) argc;
	(void) argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, ft_arrdup(envp));
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
		shell.input = readline(promt);
		free(promt);
		if (!shell.input)
		{
			if (errno)
				perror("readline failed");
			else
				ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (shell.input[0])
		{
			add_history(shell.input);
			tokenize_input(&shell);
			//parse_tokens(&shell);
			//execute_commands(&shell);
			print_tokens(&shell);
			//reset_shell(&shell);
		}
		free(shell.input);
		shell.input = NULL;
		free_tokens(shell.tokens);
		shell.tokens = NULL;
	}
	rl_clear_history();
	free_all(&shell);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2026/01/16 16:34:47 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Global signal variable - declared in signal_handler.c */
extern volatile sig_atomic_t	g_signum;

void	print_tokens(t_shell *shell)
{
	t_token	*t;

	t = shell->tokens;
	while (t != NULL)
	{
		printf("%d\n", t->type);
		printf("%s\n", t->value);
		printf("\n");
		t = t->next;
	}
}

void	print_commands(t_shell *shell)
{
	t_command	*cmd;
	int			cmd_num;

	if (!shell || !shell->commands)
	{
		printf("No commands to display.\n");
		return ;
	}
	cmd = shell->commands;
	cmd_num = 1;
	while (cmd)
	{
		printf("Command %d:\n", cmd_num);
		if (cmd->argv)
		{
			printf("  argv: ");
			for (int i = 0; cmd->argv[i]; i++)
				printf("\"%s\" ", cmd->argv[i]);
			printf("\n");
		}
		if (cmd->input_file)
			printf("  input_file: %s\n", cmd->input_file);
		if (cmd->output_file)
			printf("  output_file: %s%s\n", cmd->output_file,
				cmd->append ? " (append)" : "");
		if (cmd->heredoc_fd != -1)
			printf("  heredoc: delimiter \"%s\", fd=%d\n",
				cmd->heredoc_delim ? cmd->heredoc_delim : "", cmd->heredoc_fd);
		printf("  is_builtin: %s\n", cmd->is_builtin ? "Yes" : "No");
		printf("\n");
		cmd = cmd->next;
		cmd_num++;
	}
}

static void	reset_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->tokens)
		free_tokens(shell->tokens);
	shell->tokens = NULL;
	if (shell->commands)
		free_commands(shell->commands);
	shell->commands = NULL;
	if (shell->input)
		free(shell->input);
	shell->input = NULL;
}
/*
** Main shell loop following architecture:
** 1. Check signals
** 2. Build prompt
** 3. Read input
** 4. Check signals again
** 5. Add to history
** 6. Process (lex/parse/expand/execute)
** 7. Cleanup
*/
static void	shell_loop(t_shell *shell)
{
	char	*prompt;

	while (1)
	{
		check_signal_received(shell);
		prompt = build_prompt(shell);
		if (!prompt)
		{
			perror("minishell: failed to build prompt");
			break ;
		}
		shell->input = readline(prompt);
		free(prompt);
		if (!shell->input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}
		if (check_signal_received(shell))
		{
			free(shell->input);
			shell->input = NULL;
			continue ;
		}
		if (shell->input[0])
		{
			tokenize_input(shell);
			parse_input(shell);
			print_tokens(shell);
			print_commands(shell);
		}
		reset_shell(shell);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	**msh_envp;

	(void) argc;
	(void) argv;
	ft_bzero(&shell, sizeof(t_shell));
	msh_envp = ft_arrdup(envp);
	if (!msh_envp)
	{
		free_all(&shell);
		exit(EXIT_FAILURE);
	}
	init_shell(&shell, msh_envp);
	set_signals_interactive();
	shell_loop(&shell);
	clear_history();
	free_all(&shell);
	return (shell.last_exit);
}

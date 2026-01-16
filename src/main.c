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
	t_token	*t = shell->tokens;

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
	t_command *cmd = shell->commands;
	int i;

	while (cmd)
	{
		printf("Command:\n");
		if (cmd->argv)
		{
			i = 0;
			while (cmd->argv[i])
			{
				printf("  argv[%d]: %s\n", i, cmd->argv[i]);
				i++;
			}
		}
		if (cmd->input_file)
			printf("  input: %s\n", cmd->input_file);
		if (cmd->output_file)
			printf("  output: %s (%s)\n", cmd->output_file, cmd->append ? "append" : "trunc");
		if (cmd->heredoc_delim)
			printf("  heredoc: %s\n", cmd->heredoc_delim);
		printf("  builtin: %d\n", cmd->is_builtin);
		cmd = cmd->next;
	}
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
static void     shell_loop(t_shell *shell)
{
        char    *prompt;

        while (1)
        {
                check_signal_received(shell);
                prompt = build_prompt(shell);
                if (!prompt)
                {
                        perror("minishell: failed to build prompt\n");
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
                        /* Top-level parse; sets shell->commands and shell->last_exit on error */
                        parse_input(shell);
                        /* Debug output */
                        print_commands(shell);
                        print_tokens(shell);
                }
                free_commands(shell->commands);
                shell->commands = NULL;
                free_tokens(shell->tokens);
                shell->tokens = NULL;
                free(shell->input);
                shell->input = NULL;
        }
}

int     main(int argc, char **argv, char **envp)
{
        t_shell shell;
        //char  *promt;

	(void) argc;
	(void) argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, envp);
	set_signals_interactive();
	rl_event_hook = readline_event_hook;
	shell_loop(&shell);
	rl_clear_history();
	free_all(&shell);
	return (shell.last_exit);
}

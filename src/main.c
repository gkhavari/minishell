/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:09:51 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 15:18:18 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	disable_ctrl_echo(void)
{
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/* Global signal variable - declared in signal_handler.c */
extern volatile sig_atomic_t	g_signum;

/*
** Process input and execute command
** A stupid simple builtin testing without full parser, will be deleted next merge
*/
/*
static void     process_input(t_shell *shell)
{
        char    **argv;

        argv = simple_split_input(shell->input);
        if (!argv || !argv[0])
        {
                free_simple_argv(argv);
                return ;
        }
        if (is_builtin(argv[0]))
                shell->last_exit = run_builtin(argv, shell);
        else
        {
                ft_putstr_fd("minishell: ", STDERR_FILENO);
                ft_putstr_fd(argv[0], STDERR_FILENO);
                ft_putstr_fd(": command not found\n", STDERR_FILENO);
                shell->last_exit = 127;
        }
        free_simple_argv(argv);
}
*/
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
                        //parse_tokens(&shell);
                        //execute_commands(&shell);
                        print_tokens(shell);
                        //reset_shell(&shell);
                }
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
        init_shell(&shell, ft_arrdup(envp));
        //disable_ctrl_echo();
        shell_loop(&shell);
        rl_clear_history();
        free_all(&shell);
        return (shell.last_exit);
}

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

#include "../includes/minishell.h"

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	char	*input;
	char	*promt;

	(void) argc;
	(void) argv;
	ft_bzero(&shell, sizeof(t_shell));
	init_shell(&shell, envp);
	while (1)
	{
		promt = build_prompt(&shell);
		if (!promt)
			continue ;
		input = readline(promt);
		free(promt);
		if (!input)
		{
			perror("Error: readline\n");
			break ;
		}
		if (*input)
			add_history(input);
		free(input);
	}
	return (0);
}

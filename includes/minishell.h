/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:18:11 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/25 21:18:19 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <../libft/libft.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>

typedef struct s_shell
{
	char	**envp;			// copy of environment variables
	char	*user;			// current username
	char	cwd[256];		// current working directory
	int		last_exit;		// last command exit status
}	t_shell;

char	*build_prompt(t_shell *shell);
char	*get_env(char **envp, const char *key);
void	init_shell(t_shell *shell, char **envp);
char	*ft_strcat(char *dest, const char *src);

#endif

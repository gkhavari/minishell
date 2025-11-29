/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:13 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/29 14:11:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

/**
 * PIPE			|
 * REDIR_IN		<
 * REDIR_OUT	>
 * APPEND		>>
 * HEREDOC		<<
**/
typedef enum e_tokentype
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	APPEND,
	HEREDOC
}	t_tokentype;

typedef struct s_token
{
	t_tokentype		type;
	char			*value;		// only for WORD, others can keep NULL
	struct s_token	*next;
}	t_token;

typedef struct s_shell
{
	char	**envp;			// copy of environment variables
	char	*user;			// current username
	char	*cwd;			// current working directory
	int		last_exit;		// last command exit status
}	t_shell;

#endif

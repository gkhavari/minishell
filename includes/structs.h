/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:13 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 16:25:00 by thanh-ng         ###   ########.fr       */
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

typedef enum e_state
{
	ST_NORMAL,
	ST_SQUOTE,
	ST_DQUOTE
}	t_state;

/* Builtin command types */
typedef enum e_builtin
{
	NOT_BUILTIN = 0,
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_UNSET,
	BUILTIN_ENV,
	BUILTIN_EXIT
}	t_builtin;

typedef struct s_token
{
	t_tokentype		type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_arg
{
	char *value;
	struct s_arg *next;
}	t_arg;

typedef struct s_command
{
	t_arg				*args;			// List of execve
	char				**argv;			// Array for execve
	char				*input_file;	// Input redirection if < exists
	char				*output_file;	// Output redirection if > or >> exists
	int					append;			// Flag for >> append
	int					heredoc_fd;		// Flag for << heredoc input (-1 is invalid)
	char				*heredoc_delim;// Delimiter for heredoc 
	int					is_builtin;		// 1 if command is builtin
	struct s_command	*next;			// Next command in a pipeline
}	t_command;

typedef struct s_shell
{
	char		**envp;			// copy of environment variables
	char		*user;			// current username
	char		*cwd;			// current working directory
	int			last_exit;		// last command exit status
	t_token		*tokens;		// Tokenized input
	t_command	*commands;		// Parsed commands
	char		*input;			// Raw user input
}	t_shell;

#endif

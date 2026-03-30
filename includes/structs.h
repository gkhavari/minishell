/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:13 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/30 16:46:10 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

typedef enum e_tokentype
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	APPEND,
	HEREDOC,
}	t_tokentype;

typedef enum e_state
{
	ST_NORMAL,
	ST_SQUOTE,
	ST_DQUOTE
}	t_state;

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
	t_tokentype			type;
	char				*value;
	int					quoted;
	struct s_token		*next;
}	t_token;

typedef struct s_arg
{
	char				*value;
	struct s_arg		*next;
}	t_arg;

typedef struct s_redir
{
	char				*file;
	int					fd;
	int					append;
	struct s_redir		*next;
}	t_redir;

typedef struct s_command
{
	t_arg				*args;
	char				**argv;
	t_redir				*redirs;
	int					heredoc_fd;
	char				*heredoc_delim;
	int					heredoc_quoted;
	int					is_builtin;
	struct s_command	*next;
}	t_command;

typedef struct s_shell
{
	char				**envp;
	char				*user;
	char				*cwd;
	int					last_exit;
	int					had_path;
	int					barrier_write_fd;
	t_token				*tokens;
	t_command			*commands;	
	char				*input;
	int					word_quoted;
	int					heredoc_mode;
	int					stdin_backup;
	int					stdout_backup;
}	t_shell;

#endif

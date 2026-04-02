/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:13 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 13:54:13 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

# include <../libft/libft.h>

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

typedef enum e_builtin
{
	B_NONE = 0,
	B_ECHO,
	B_CD,
	B_PWD,
	B_EXPORT,
	B_UNSET,
	B_ENV,
	B_EXIT,
	B_COUNT
}	t_builtin;

typedef struct s_token
{
	t_tokentype			type;
	char				*value;
	int					quoted;
}	t_token;

typedef struct s_arg
{
	char				*value;
}	t_arg;

typedef struct s_redir
{
	char				*file;
	int					fd;
	int					append;
}	t_redir;

typedef struct s_command
{
	t_list				*args;
	char				**argv;
	t_list				*redirs;
	int					hd_fd;
	char				*hd_delim;
	int					hd_quoted;
	int					stdin_last;
	int					is_builtin;
}	t_command;

typedef struct s_shell
{
	char				**envp;
	char				*user;
	char				*cwd;
	int					last_exit;
	int					had_path;
	int					barrier_write_fd;
	t_list				*tokens;
	t_list				*cmds;
	char				*input;
	int					word_quoted;
	int					hd_mod;
	int					oom;
}	t_shell;

typedef struct s_builtin_reg
{
	const char	*name;
	int			(*run)(char **, t_shell *);
}	t_builtin_reg;

typedef struct s_heredoc_ctx
{
	t_command	*cmd;
	t_shell		*shell;
	int			pipe_fd[2];
	int			expand;
}	t_heredoc_ctx;

#endif

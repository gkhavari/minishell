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

/*
** Token kinds produced by the tokenizer:
**   WORD      — plain word or (partially) quoted string
**   PIPE      — |
**   REDIR_IN  — <    REDIR_OUT — >    APPEND — >>    HEREDOC — <<
*/
typedef enum e_tokentype
{
	WORD,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	APPEND,
	HEREDOC
}	t_tokentype;

/*
** Tokenizer quote-state machine:
**   ST_NORMAL — outside any quotes
**   ST_SQUOTE — inside single quotes (no expansion)
**   ST_DQUOTE — inside double quotes ($ expansion only)
*/
typedef enum e_state
{
	ST_NORMAL,
	ST_SQUOTE,
	ST_DQUOTE
}	t_state;

/*
** Builtin command identifiers.
** B_NONE (0) means the command is external (run via execve).
** B_COUNT is a sentinel equal to the number of registered builtins.
*/
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

/*
** One token produced by the tokenizer (lives in shell->tokens).
** value  — heap string; NULL for pure operators (PIPE etc.)
** quoted — 1 if the word was (partially) quoted (suppresses field split)
*/
typedef struct s_token
{
	t_tokentype			type;
	char				*value;
	int					quoted;
}	t_token;

/*
** One command argument; content of t_command->args nodes.
** value — fully expanded argument string.
*/
typedef struct s_arg
{
	char				*value;
}	t_arg;

/*
** One redirection; content of t_command->redirs nodes (source order).
** file   — filename (or S_AMBIG sentinel for ambiguous redirect)
** fd     — STDIN_FILENO (<) or STDOUT_FILENO (> / >>)
** append — 1 = O_APPEND (>>),  0 = O_TRUNC (>)
*/
typedef struct s_redir
{
	char				*file;
	int					fd;
	int					append;
}	t_redir;

/*
** One command in a pipeline (content of shell->cmds nodes).
** args      — raw word list (t_arg); replaced by argv after finalize
** argv      — NULL-terminated argv built by finalize_cmds()
** redirs    — t_redir nodes in source order
** hd_fd     — read end of heredoc pipe; -1 if no heredoc
** hd_delim  — last << delimiter string; NULL if no heredoc
** hd_quoted — 1 when delimiter was quoted (suppresses $-expansion)
** stdin_last — STDIN_LAST_* — which stdin redirect came last in source
** is_builtin — t_builtin id; B_NONE (0) means external command
*/
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

/*
** Global shell state; one instance for the process lifetime.
** envp       — NULL-terminated env array (heap, KEY=value strings)
** user / cwd — $USER / $PWD at startup (used in prompt)
** last_exit  — $? : exit status of the last finished command
** had_path   — 1 if PATH existed in envp at startup
** tokens     — tokenizer output; freed after parse
** cmds       — parser output; freed after execution
** input      — current line from readline / stdin
** word_quoted — 1 while building a (partially) quoted token
** hd_mod     — 1 after << so delimiter word is not $-expanded
** oom        — 1 after any tokenizer/parser malloc failure
*/
typedef struct s_shell
{
	char				**envp;
	char				*user;
	char				*cwd;
	int					last_exit;
	int					had_path;
	t_list				*tokens;
	t_list				*cmds;
	char				*input;
	int					word_quoted;
	int					hd_mod;
	int					oom;
}	t_shell;

/*
** Entry in the builtin dispatch table (builtin_dispatcher.c).
** name — argv[0] string to match
** run  — builtin function pointer: run(argv, shell)
*/
typedef struct s_builtin_reg
{
	const char	*name;
	int			(*run)(char **, t_shell *);
}	t_builtin_reg;

/*
** Context threaded through the heredoc reader helpers.
** cmd     — command that owns this heredoc
** shell   — for oom flag, env, and signal checks
** pipe_fd — [0] read end stored in cmd->hd_fd; [1] write end for lines
** expand  — 1 when $-expansion is enabled (unquoted delimiter)
*/
typedef struct s_heredoc_ctx
{
	t_command	*cmd;
	t_shell		*shell;
	int			pipe_fd[2];
	int			expand;
}	t_heredoc_ctx;

#endif

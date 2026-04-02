/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:10:46 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/29 14:10:48 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

/*
** General 0/1 outcomes. SUCCESS and FAILURE are aliases of OK and ERR.
*/
# define OK		0
# define ERR		1
# define SUCCESS	OK
# define FAILURE	ERR
/* Structural parse error in command build; not heap OOM (see OOM). */
# define PARSE_ERR -1
# define OOM		-2

/*
 * Logical predicates (int 0/1). Not OK/ERR/OOM.
 * Use in is_* / needs_* helpers instead of raw 0/1.
 */
# define FALSE	0
# define TRUE	1

/*
 * Lexer handler: no-op vs handled input. Check OOM before treating as bool.
 */
# define LEX_NO		0
# define LEX_YES	1

/*PROMT SYNTAX*/
# define PROMPT_PREFIX "@minishell:"
# define PROMPT_SUFFIX "$ "
# define PROMPT_DEFAULT_USER "user"
# define PROMPT_DEFAULT_CWD "/minishell/"

/* read_input status — not $? (see AGENTS.md). */
# define READ_LINE	1
# define READ_EOF	0
# define READ_SIG	-1

/*
 * Exit status — bash / POSIX wait(2) conventions:
 *
 *   0              Success.
 *   1              General error (OOM, parse failures, etc.).
 *   2              Shell syntax error or invalid builtin usage.
 *   126            Command invoked but cannot execute (is a directory, etc.).
 *   127            Command not found (or equivalent).
 *   128 + n        Terminated by signal n; e.g. SIGINT (2) -> 130, SIGQUIT (3)
 *                  -> 131, SIGTERM (15) -> 143. Use EXIT_STATUS_FROM_SIGNAL
 *                  with WTERMSIG(status) from waitpid.
 *
 * (Do not redefine EXIT_SUCCESS / EXIT_FAILURE from <stdlib.h>; use SUCCESS /
 *  FAILURE for the same numeric values where we avoid pulling stdlib
 *  into headers that do not need it.)
 */
# define EXIT_SYNTAX_ERROR		2
# define EXIT_CMD_CANNOT_EXECUTE	126
# define EXIT_CMD_NOT_FOUND		127
# define EXIT_STATUS_SIGNAL_BASE	128
# define EXIT_STATUS_FROM_SIGNAL(sig)	((EXIT_STATUS_SIGNAL_BASE) + (sig))
/* SIGINT from <signal.h>; include that before defines.h. */
# define EXIT_SIGINT			EXIT_STATUS_FROM_SIGNAL(SIGINT)

/*
 * Parser: tokens consumed per add_token / redir step. Not $?.
 */
# define PARSE_ONE	1
# define PARSE_PAIR	2

/* Heredoc line reader (read_one / consume_line). */
# define HD_MORE	0
# define HD_DELIM	1
# define HD_EOF		2

/* Program name for stderr (BEHAVIOR / subject). */
# define SH_NAME	"minishell"

/* Internal WORD sentinels (lexer/parser). */
# define EMPTY_EXPAND	"\001msh_empty"
# define AMBIG_REDIR	"\001msh_ambig:"

/*
** Path buffer length for resolve/exec helpers. <limits.h> (via includes.h)
** normally defines PATH_MAX; keep a fallback for unusual toolchains.
*/
# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

#endif

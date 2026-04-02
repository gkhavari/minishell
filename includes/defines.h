/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:10:46 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

/*
** General 0/1 outcomes. SUCCESS and FAILURE are aliases of OK and ERR.
*/
# define OK		    0
# define ERR		1
# define SUCCESS	OK
# define FAILURE	ERR
/* Structural parse error in command build; not heap OOM (see OOM). */
# define PR_ERR		-1
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
# define LX_N		0
# define LX_Y		1

/* Prompt pieces (readline). */
# define PM_PFX		"@minishell:"
# define PM_SFX		"$ "
# define PM_USR		"user"
# define PM_CWD		"/minishell/"

/* read_input status — not $? (see AGENTS.md). */
# define RL_LN		1
# define RL_EOF		0
# define RL_SIG		-1

/*
 * Exit status — bash / POSIX wait(2) conventions:
 *
 *   0              Success.
 *   1              General error (OOM, parse failures, etc.).
 *   2              Shell syntax error or invalid builtin usage.
 *   126            Command invoked but cannot execute (is a directory, etc.).
 *   127            Command not found (or equivalent).
 *   128 + n        Terminated by signal n; e.g. SIGINT (2) -> 130, SIGQUIT (3)
 *                  -> 131, SIGTERM (15) -> 143. Use XSIG with WTERMSIG(status).
 *
 * (Do not redefine EXIT_SUCCESS / EXIT_FAILURE from <stdlib.h>; use SUCCESS /
 *  FAILURE for the same numeric values where we avoid pulling stdlib
 *  into headers that do not need it.)
 */
# define XSYN		2
# define XNX		126
# define XNF		127
# define XSB		128
# define XSIG(sig)	((XSB) + (sig))
/* SIGINT from <signal.h>; include that before defines.h. */
# define XSINT		XSIG(SIGINT)

/*
 * Parser: tokens consumed per add_token / redir step. Not $?.
 */
# define PR_1		1
# define PR_2		2

/* Heredoc line reader (read_one / consume_line). */
# define HD_MORE	0
# define HD_DELIM	1
# define HD_EOF		2

/* Program name for stderr (BEHAVIOR / subject). */
# define SH_NAME	"minishell"

/* Internal WORD sentinels (lexer/parser). */
# define S_EMPTY	"\001msh_empty"
# define S_AMBIG	"\001msh_ambig:"

/*
** Path buffer length for resolve/exec helpers. <limits.h> (via includes.h)
** normally defines PATH_MAX; keep a fallback for unusual toolchains.
*/
# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

/* --- Long names (docs / grep); alias to abbreviations above --- */
# define PARSE_ERR		PR_ERR
# define LEX_NO			LX_N
# define LEX_YES		LX_Y
# define PROMPT_PREFIX		PM_PFX
# define PROMPT_SUFFIX		PM_SFX
# define PROMPT_DEFAULT_USER	PM_USR
# define PROMPT_DEFAULT_CWD	PM_CWD
# define READ_LINE		RL_LN
# define READ_EOF		RL_EOF
# define READ_SIG		RL_SIG
# define EXIT_SYNTAX_ERROR	XSYN
# define EXIT_CMD_CANNOT_EXECUTE	XNX
# define EXIT_CMD_NOT_FOUND	XNF
# define EXIT_STATUS_SIGNAL_BASE	XSB
# define EXIT_STATUS_FROM_SIGNAL	XSIG
# define EXIT_SIGINT		XSINT
# define PARSE_ONE		PR_1
# define PARSE_PAIR		PR_2
# define EMPTY_EXPAND		S_EMPTY
# define AMBIG_REDIR		S_AMBIG

#endif

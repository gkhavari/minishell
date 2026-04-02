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

# define SUCCESS   0
# define FAILURE   1
# define PARSE_ERR -1
# define MSH_OOM   -2

/*
 * Lexer / expansion step: not applicable vs consumed input.
 * Check MSH_OOM before treating as boolean. Not shell exit codes.
 */
# define MSH_LEX_NO  0
# define MSH_LEX_YES 1

/*PROMT SYNTAX*/
# define PROMPT_PREFIX "@minishell:"
# define PROMPT_SUFFIX "$ "
# define PROMPT_DEFAULT_USER "user"
# define PROMPT_DEFAULT_CWD "/minishell/"

# define SINGLE_QUOTE 39
# define DOUBLE_QUOTE 34
/*
 * ASCII DEL (display filter); value equals EXIT_CMD_NOT_FOUND
 * but not exit API.
 */
# define MSH_ASCII_DEL 127

/* read_input status — not $? / exit codes (see AGENTS.md §9) */
# define MSH_READ_LINE 1
# define MSH_READ_EOF 0
# define MSH_READ_SIG -1

/*SYNTAX CHECK*/
# define SYNTAX_OK 0
# define SYNTAX_ERR 1

/*
 * Exit status — bash / POSIX wait(2) conventions:
 *
 *   0               Success.
 *   1               General error (OOM, parse failures, etc.).
 *   2               Shell syntax error or invalid builtin usage.
 *   126             Command invoked but cannot execute (is a directory, etc.).
 *   127             Command not found (or equivalent).
 *   128 + n         Terminated by signal n; e.g. SIGINT (2) -> 130, SIGQUIT (3)
 *                   -> 131, SIGTERM (15) -> 143. Use EXIT_STATUS_FROM_SIGNAL
 *                   with WTERMSIG(status) from waitpid.
 *
 * (Do not redefine EXIT_SUCCESS / EXIT_FAILURE from <stdlib.h>; use SUCCESS /
 *  FAILURE below for the same numeric values where we avoid pulling stdlib
 *  into headers that do not need it.)
 */
# define EXIT_SYNTAX_ERROR		2
# define EXIT_CMD_CANNOT_EXECUTE	126
# define EXIT_CMD_NOT_FOUND		127
# define EXIT_STATUS_SIGNAL_BASE	128
# define EXIT_STATUS_FROM_SIGNAL(sig)	((EXIT_STATUS_SIGNAL_BASE) + (sig))
/* SIGINT from <signal.h>; include that before defines.h (minishell.h order). */
# define EXIT_SIGINT			EXIT_STATUS_FROM_SIGNAL(SIGINT)

/* Shell name for stderr lines (see BEHAVIOR / subject). */
# define MSH_NAME "minishell"

/*INTERNAL TOKENS*/
# define MSH_EMPTY_EXPAND_TOKEN "\001msh_empty"
# define MSH_AMBIG_REDIR_PREFIX "\001msh_ambig:"

/*
** Path buffer length for resolve/exec helpers. <limits.h> (via includes.h)
** normally defines PATH_MAX; keep a fallback for unusual toolchains.
*/
# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

#endif

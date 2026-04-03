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
** General function outcomes. Use OOM (-2) when malloc fails and the caller
** must unwind differently from a plain FAILURE (1).
*/
# define SUCCESS	0	/* operation completed without error          */
# define FAILURE	1	/* general / recoverable error                */
# define PR_ERR		-1	/* parse error: structural problem in cmd build */
# define OOM		-2	/* out of memory: malloc/calloc returned NULL */

/*
** Logical booleans (int 0/1). Distinct from SUCCESS/FAILURE so predicate
** helpers read naturally: `if (is_blank(...))` vs `if (flush == SUCCESS)`.
*/
# define FALSE		0
# define TRUE		1

/*
** Which stdin redirect appeared last in source order (that one wins).
** Stored on t_command during parse; apply_redirs uses it with hd_fd to
** pick the stdin source.
*/
# define STDIN_LAST_NONE	0	/* no stdin redirect on this command  */
# define STDIN_LAST_HD		1	/* last stdin was << (heredoc)        */
# define STDIN_LAST_FILE	2	/* last stdin was < (file redirect)   */

/*
** Tokenizer handler return codes. A handler either claims the current
** position (TOK_Y) or passes (TOK_N). OOM is distinct — always check
** for OOM before treating the result as a boolean.
*/
# define TOK_N		0	/* TOK No  — handler did not consume this char */
# define TOK_Y		1	/* TOK Yes — handler consumed and advanced *i  */

/* Prompt pieces assembled by build_prompt() in repl_loop.c. */
# define PM_PFX		"@minishell:"	/* prefix between user and cwd  */
# define PM_SFX		"$ "		/* suffix shown after cwd        */
# define PM_USR		"user"		/* fallback when $USER is unset  */
# define PM_CWD		"/minishell/"	/* fallback when $PWD is unset   */

/*
** Return codes from read_input() / ft_read_stdin_line().
** RL_LN  — a line was read successfully.
** RL_EOF — readline returned NULL (Ctrl-D or end of non-TTY input).
** RL_SIG — SIGINT was delivered during readline (Ctrl-C).
*/
# define RL_LN		1	/* Read Line: got a line              */
# define RL_EOF		0	/* Read EOF:  readline returned NULL  */
# define RL_SIG		-1	/* Read Sig:  interrupted by SIGINT   */

/*
** Exit status layout (same numbering as wait(2) status):
**   0          Success.
**   1          General error (OOM, internal failures).
**   2          Shell syntax error or invalid builtin usage.
**   126        Command found but cannot execute (directory, no x-bit).
**   127        Command not found.
**   128 + n    Killed by signal n  (SIGINT=2 → 130, SIGQUIT=3 → 131).
**
** Do NOT redefine EXIT_SUCCESS / EXIT_FAILURE from <stdlib.h>; use
** SUCCESS / FAILURE (same numeric values) for files that skip stdlib.
*/
# define XSYN		2	/* exit: syntax error                      */
# define XNX		126	/* exit: cmd Not eXecutable                */
# define XNF		127	/* exit: cmd Not Found                     */
# define XSB		128	/* exit: signal base (128 + signal number) */
/* exit code when killed by SIGINT (128 + SIGINT = 130) */
# define XSINT		130

/*
** Parser token-advance counts returned by add_token_to_command() and
** parse_redir_token_pair(). Tells build_command_list() how many list
** nodes to skip after consuming a token.
*/
# define PR_1		1	/* Parse 1: consumed one token  (word)         */
# define PR_2		2	/* Parse 2: consumed two tokens (redir + file) */

/*
** Heredoc line-reader status (heredoc_read_one / heredoc_consume_line).
*/
# define HD_MORE	0	/* Heredoc More:  line written, keep reading   */
# define HD_DELIM	1	/* Heredoc Delim: delimiter matched, stop      */
# define HD_EOF		2	/* Heredoc EOF:   stdin ended before delimiter */

/* Program name used as prefix for all stderr error messages. */
# define SH_NAME	"minishell"

/*
** Internal WORD sentinels injected by the tokenizer to carry special
** meaning through the parser without extra struct fields.
**
** S_EMPTY (\001msh_empty)   — placeholder for a $VAR that expanded to
**   nothing in a position where a word is expected (e.g. `echo $UNSET`).
**   The parser skips it so it does not become an argv entry.
**
** S_AMBIG (\001msh_ambig:raw) — used when $VAR expanded to empty in a
**   redirection filename position.  The raw text after the colon lets
**   apply_redirs emit the correct "ambiguous redirect" message.
*/
# define S_EMPTY	"\001msh_empty"		/* empty-expansion placeholder */
# define S_AMBIG	"\001msh_ambig:"	/* ambiguous-redirect prefix   */

/*
** PATH_MAX: maximum byte length of a filesystem path.
** Normally provided by <limits.h>; the fallback covers unusual toolchains.
*/
# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

#endif

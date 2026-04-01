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

/*
 * MSH_OOM: propagate malloc failure up to a safe frame (e.g. tokenize_input,
 * process_input) — do not clean_exit() from deep helpers; unwind and free there.
 * Negative like PARSE_ERR (-1); avoids clashing with read_operator() return 1–2.
 */
# define MSH_OOM   -2

/* read_input() return values (see main.c) */
# define READ_INPUT_OOM 2

/*PROMT SYNTAX*/
# define PROMPT_PREFIX "@minishell:"
# define PROMPT_SUFFIX "$ "
# define PROMPT_DEFAULT_USER "user"
# define PROMPT_DEFAULT_CWD "/minishell/"

# define SINGLE_QUOTE 39
# define DOUBLE_QUOTE 34

/*SYNTAX CHECK*/
# define SYNTAX_OK 0
# define SYNTAX_ERR 1

/*EXIT CODES*/
# define EXIT_SYNTAX_ERROR 2
# define EXIT_SIGINT 130

/*INTERNAL TOKENS*/
# define MSH_EMPTY_EXPAND_TOKEN "\001msh_empty"
# define MSH_AMBIG_REDIR_PREFIX "\001msh_ambig:"

#endif

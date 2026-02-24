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

# define SUCESS 0
# define FAILURE -1

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

#endif

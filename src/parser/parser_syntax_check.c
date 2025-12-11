/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax_check.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:00:39 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:00:41 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	syntax_check(t_token *token)
{
	if (token && token->type == PIPE)
		return (syntax_error("|"));
	while (token)
	{
		if (token->type == PIPE && (!token->next || token->next->type == PIPE))
			return (syntax_error("|"));
		if ((token->type == REDIR_IN || token->type == REDIR_OUT
				|| token->type == APPEND || token->type == HEREDOC)
			&& (!token->next || token->next->type != WORD))
			return (syntax_error("newline"));
		token = token->next;
	}
	return (0);
}

int	syntax_error(const char *msg)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("'\n", 2);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:25:57 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 17:38:11 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * free_tokens - Free memory allocated for token list
 * @token: Pointer to the first token in the list
 */
void	free_tokens(t_token *token)
{
	t_token	*tmp;

	while (token)
	{
		tmp = token->next;
		if (token->value)
			free(token->value);
		free(token);
		token = tmp;
	}
}

/**
 * free_args - Free memory allocated for argument list
 * @arg: Pointer to the first argument in the list
 */
void	free_args(t_arg *arg)
{
	t_arg	*tmp;

	while (arg)
	{
		tmp = arg->next;
		if (arg->value)
			free(arg->value);
		free(arg);
		arg = tmp;
	}
}

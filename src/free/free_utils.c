/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:25:57 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 20:54:52 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Free a linked list of tokens and their allocated contents.

 BEHAVIOR:
* Iterates the token list, frees each token's `value` string (if present)
* and the token structure itself.

 PARAMETERS:
* t_token *token: Pointer to the first token in the list; may be NULL.
 
 RETURN:
 * None.
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
 DESCRIPTION:
* Free a linked list of argument nodes and their stored strings.

 BEHAVIOR:
* Iterates the `t_arg` list, frees each `value` string and the node.

 PARAMETERS:
* t_arg *arg: Pointer to the first argument node; may be NULL.
 
 RETURN:
 * None.
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

/**
 DESCRIPTION:
* Free a NULL-terminated array of strings.

 BEHAVIOR:
* Iterates the array, frees each element and then frees the array
* pointer itself. Safe to call with NULL.

 PARAMETERS:
* char **array: NULL-terminated array to free.

 RETURN:
* None.
*/
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

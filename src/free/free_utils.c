/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** `ft_lstclear` del: free `t_token` value string and struct. */
static void	del_token_content(void *content)
{
	t_token	*t;

	t = content;
	if (t->value)
		free(t->value);
	free(t);
}

/** `ft_lstclear` del: free `t_arg` value string and struct. */
static void	del_arg_content(void *content)
{
	t_arg	*a;

	a = content;
	if (a->value)
		free(a->value);
	free(a);
}

/** Clear token `t_list`; each node content is `t_token *`. */
void	free_tokens(t_list **lst)
{
	if (!lst || !*lst)
		return ;
	ft_lstclear(lst, del_token_content);
}

/** Clear argument `t_list`; each node content is `t_arg *`. */
void	free_args(t_list **lst)
{
	if (!lst || !*lst)
		return ;
	ft_lstclear(lst, del_arg_content);
}

/** Free `*p` and set it to NULL (lexer word buffer, etc.). */
void	msh_strptr_free(char **p)
{
	if (p && *p)
		free(*p);
	if (p)
		*p = NULL;
}

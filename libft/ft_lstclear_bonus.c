/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:54:22 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:54:24 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_bonus.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*head;
	t_list	*temp;

	if (del == NULL)
		return ;
	head = *lst;
	while (head)
	{
		temp = head -> next;
		del(head -> content);
		free(head);
		head = temp;
	}
	*lst = NULL;
}
/*
void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	if (*lst == NULL)
		return ;
	ft_lstclear(&(*lst) -> next, del);
	del ((*lst) -> content);
	free (*lst);
	*lst = NULL;
}

void ft_lstclear(t_list **lst, void (*del)(void *))
{
    t_list *temp;

    while (*lst)
    {
        temp = (*lst)->next;
        del((*lst)->content);
        free(*lst);
        *lst = temp;
    }
}
*/

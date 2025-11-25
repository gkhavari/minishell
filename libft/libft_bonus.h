/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:06:10 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:06:12 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_BONUS_H
# define LIBFT_BONUS_H

# include <unistd.h>
# include <stdlib.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}		t_list;

t_list	*ft_lstnew_bonus(void *content);
void	ft_lstadd_front_bonus(t_list **lst, t_list *new);
int		ft_lstsize_bonus(t_list *lst);
t_list	*ft_lstlast_bonus(t_list *lst);
void	ft_lstadd_back_bonus(t_list **lst, t_list *new);
void	ft_lstdelone_bonus(t_list *lst, void (*del)(void*));
void	ft_lstclear_bonus(t_list **lst, void (*del)(void*));
void	ft_lstiter_bonus(t_list *lst, void (*f)(void *));
t_list	*ft_lstmap_bonus(t_list *lst, void *(*f)(void *), void (*del)(void *));

#endif

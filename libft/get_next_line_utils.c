/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:57:12 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 21:15:02 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	has_newline(t_buffer *stash)
{
	t_buffer	*last;
	int			i;

	if (!stash)
		return (0);
	last = get_last_buffer(stash);
	i = last->position;
	while (i < last->size)
	{
		if (last->content[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

int	calculate_line_size(t_buffer *stash)
{
	t_buffer	*current;
	int			len;
	int			i;

	len = 0;
	current = stash;
	while (current)
	{
		i = current->position;
		while (i < current->size)
		{
			len++;
			if (current->content[i] == '\n')
				break ;
			i++;
		}
		current = current->next;
	}
	return (len);
}

void	free_stash_list(t_buffer *stash)
{
	t_buffer	*current;
	t_buffer	*next;

	current = stash;
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
}

int	add_buffer_to_stash(t_buffer **stash, char *buf, int bytes_read)
{
	(void)stash;
	(void)buf;
	(void)bytes_read;
	return (ERROR);
}

t_buffer	*create_new_buffer(char *buf, int bytes_read)
{
	(void)buf;
	(void)bytes_read;
	return (NULL);
}

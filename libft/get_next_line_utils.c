/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:57:12 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 20:57:13 by thanh-ng         ###   ########.fr       */
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

t_buffer	*get_last_buffer(t_buffer *stash)
{
	t_buffer	*current;

	if (!stash)
		return (NULL);
	current = stash;
	while (current->next)
		current = current->next;
	return (current);
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
	t_buffer	*new_node;
	t_buffer	*last;

	new_node = create_new_buffer(buf, bytes_read);
	if (!new_node)
		return (ERROR);
	if (!*stash)
		*stash = new_node;
	else
	{
		last = get_last_buffer(*stash);
		last->next = new_node;
	}
	return (SUCCESS);
}

t_buffer	*create_new_buffer(char *buf, int bytes_read)
{
	t_buffer	*new_node;
	int			i;

	new_node = malloc(sizeof(t_buffer));
	if (!new_node)
		return (NULL);
	new_node->position = 0;
	new_node->size = bytes_read;
	new_node->next = NULL;
	i = 0;
	while (i < bytes_read)
	{
		new_node->content[i] = buf[i];
		i++;
	}
	new_node->content[bytes_read] = '\0';
	return (new_node);
}

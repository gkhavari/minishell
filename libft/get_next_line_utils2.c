/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 21:08:29 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 21:15:56 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>

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

int	read_to_stash(int fd, t_buffer **stash)
{
	int	bytes_read;

	bytes_read = 1;
	while (!has_newline(*stash) && bytes_read > 0)
	{
		if (read_one_chunk(fd, stash, &bytes_read) == ERROR)
			return (ERROR);
	}
	return (SUCCESS);
}

static int	read_one_chunk(int fd, t_buffer **stash, int *bytes_read)
{
	char	*buffer;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (ERROR);
	*bytes_read = read(fd, buffer, BUFFER_SIZE);
	if (*bytes_read < 0)
	{
		free(buffer);
		return (ERROR);
	}
	if (*bytes_read > 0)
	{
		buffer[*bytes_read] = '\0';
		if (add_buffer_to_stash(stash, buffer, *bytes_read) == -1)
		{
			free(buffer);
			return (ERROR);
		}
	}
	free(buffer);
	return (SUCCESS);
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

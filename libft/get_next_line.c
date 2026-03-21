/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:57:17 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 21:15:29 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*gnl_clear_and_return_null(t_buffer **stash_ptr, char *line)
{
	free_stash_list(*stash_ptr);
	*stash_ptr = NULL;
	if (line)
		free(line);
	return (NULL);
}

char	*get_next_line(int fd)
{
	static t_buffer	*stash;
	char			*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, NULL, 0) < 0)
		return (gnl_clear_and_return_null(&stash, NULL));
	if (read_to_stash(fd, &stash) == ERROR)
		return (gnl_clear_and_return_null(&stash, NULL));
	if (!stash)
		return (NULL);
	line = build_line_from_stash(stash);
	if (cleanup_stash(&stash) == ERROR)
		return (gnl_clear_and_return_null(&stash, line));
	if (!line || line[0] == '\0')
		return (gnl_clear_and_return_null(&stash, line));
	return (line);
}

/* read_to_stash moved to get_next_line_utils2.c to reduce functions per file */

static void	append_from_node(t_buffer *current, char *line, int *j)
{
	int	i;

	i = current->position;
	while (i < current->size)
	{
		line[(*j)++] = current->content[i];
		if (current->content[i++] == '\n')
			break ;
	}
}

char	*build_line_from_stash(t_buffer *stash)
{
	t_buffer	*current;
	char		*line;
	int			j;

	if (!stash)
		return (NULL);
	line = malloc(calculate_line_size(stash) + 1);
	if (!line)
		return (NULL);
	current = stash;
	j = 0;
	while (current)
	{
		append_from_node(current, line, &j);
		current = current->next;
	}
	line[j] = '\0';
	return (line);
}

int	cleanup_stash(t_buffer **stash)
{
	t_buffer	*last;
	t_buffer	*new_node;
	int			i;

	if (!stash || !*stash)
		return (SUCCESS);
	last = get_last_buffer(*stash);
	i = last->position;
	while (i < last->size && last->content[i] != '\n')
		i++;
	if (i < last->size && last->content[i] == '\n')
		i++;
	if (i >= last->size)
	{
		free_stash_list(*stash);
		*stash = NULL;
		return (0);
	}
	new_node = create_new_buffer(last->content + i, last->size - i);
	if (!new_node)
		return (ERROR);
	free_stash_list(*stash);
	*stash = new_node;
	return (SUCCESS);
}

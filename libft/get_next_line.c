#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static t_buffer	*stash;
	char			*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, NULL, 0) < 0)
		return (free_stash_list(stash), NULL);
	if (read_to_stash(fd, &stash) == ERROR)
		return (free_stash_list(stash), NULL);
	if (!stash)
		return (NULL);
	line = build_line_from_stash(stash);
	if (cleanup_stash(&stash) == ERROR)
		return (free_stash_list(stash), free(line), NULL);
	if (!line || (line && line[0] == '\0'))
		return (free_stash_list(stash), free(line), NULL);
	return (line);
}

int	read_to_stash(int fd, t_buffer **stash)
{
	char	*buffer;
	int		bytes_read;

	bytes_read = 1;
	while (!has_newline(*stash) && bytes_read > 0)
	{
		buffer = (char *)malloc(BUFFER_SIZE + 1);
		if (!buffer)
			return (ERROR);
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (free(buffer), ERROR);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			if (add_buffer_to_stash(stash, buffer, bytes_read) == -1)
				return (free(buffer), ERROR);
		}
		free(buffer);
	}
	return (SUCCESS);
}

char	*build_line_from_stash(t_buffer *stash)
{
	t_buffer	*current;
	char		*line;
	int			i;
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
		i = current->position;
		while (i < current->size)
		{
			line[j++] = current->content[i];
			if (current->content[i++] == '\n')
				break ;
		}
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
		return (free_stash_list(*stash), *stash = NULL, 0);
	new_node = create_new_buffer(last->content + i, last->size - i);
	if (!new_node)
		return (ERROR);
	free_stash_list(*stash);
	*stash = new_node;
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:54:10 by gkhavari          #+#    #+#             */
/*   Updated: 2025/06/10 11:58:40 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*fill_line(int fd, char *new_line, char *buffer);
char	*setup_line(char *line_buffer);
char	*free_and_return(char *to_free, char *to_return);

char	*get_next_line(int fd)
{
	char		*buffer;
	char		*next_line;
	static char	*left_over_chars = NULL;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (free_and_return(left_over_chars, NULL));
	buffer = (char *)ft_calloc(BUFFER_SIZE + 1, sizeof(char));
	if (!buffer)
		return (free_and_return(left_over_chars, NULL));
	next_line = fill_line(fd, left_over_chars, buffer);
	free(buffer);
	if (!next_line)
		return (free_and_return(left_over_chars, NULL));
	left_over_chars = setup_line(next_line);
	if (!next_line[0])
		return (free_and_return(next_line, NULL));
	return (next_line);
}

char	*fill_line(int fd, char *new_line, char *buffer)
{
	int		bytes_read;

	bytes_read = 0;
	while (bytes_read >= 0)
	{
		if (new_line && ft_strchr(new_line, '\n'))
			break ;
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == 0)
			return (new_line);
		else if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			if (!new_line)
				new_line = ft_strjoin("", buffer);
			else
				new_line = free_and_return(new_line,
						ft_strjoin(new_line, buffer));
		}
		else
			return (NULL);
	}
	return (new_line);
}

char	*setup_line(char *line_buffer)
{
	size_t	i;
	char	*left_over_chars;

	i = 0;
	while (line_buffer[i] != '\n' && line_buffer[i] != '\0')
		i++;
	if (line_buffer[i] == '\n')
	{
		left_over_chars = ft_substr(line_buffer, i + 1,
				ft_strlen(line_buffer) - i);
		if (!left_over_chars)
			return (NULL);
		line_buffer[i + 1] = '\0';
		return (left_over_chars);
	}
	else if (line_buffer[i] == '\0')
		return (NULL);
	else
		return (NULL);
}

char	*free_and_return(char *to_free, char *to_return)
{
	free(to_free);
	return (to_return);
}

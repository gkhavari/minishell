/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:57:25 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 20:57:26 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include <stdlib.h>
# include <unistd.h>

# define SUCCESS 0
# define ERROR -1

typedef struct s_buffer
{
	char			content[BUFFER_SIZE + 1];
	int				position;
	int				size;
	struct s_buffer	*next;
}	t_buffer;

char		*get_next_line(int fd);
int			has_newline(t_buffer *stash);
int			read_to_stash(int fd, t_buffer **stash);
int			add_buffer_to_stash(t_buffer **stash, char *buf, int bytes_read);
char		*build_line_from_stash(t_buffer *stash);
int			calculate_line_size(t_buffer *stash);
int			cleanup_stash(t_buffer **stash);
void		free_stash_list(t_buffer *stash);
t_buffer	*get_last_buffer(t_buffer *stash);
t_buffer	*create_new_buffer(char *buf, int bytes_read);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 11:49:10 by gkhavari          #+#    #+#             */
/*   Updated: 2025/06/10 11:49:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

char	*get_next_line(int fd);
size_t	ft_strlen(const char *str);
char	*ft_substr(char *str, size_t start, size_t end);
char	*ft_strjoin(char const *str1, char const *str2);
char	*ft_strchr(const char *str, int c);
void	*ft_calloc(size_t nmemb, size_t size);

#endif // GET_NEXT_LINE_H

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 15:56:01 by gkhavari          #+#    #+#             */
/*   Updated: 2025/06/10 11:51:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void			*r;
	size_t			total_size;
	size_t			i;
	unsigned char	*ptr;

	total_size = nmemb * size;
	i = 0;
	r = (int *)malloc(total_size);
	if (r == NULL)
		return (NULL);
	ptr = (unsigned char *)r;
	while (i < nmemb)
	{
		ptr[i] = 0;
		i++;
	}
	return (r);
}

size_t	ft_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (str[len] != '\0')
		len++;
	return (len);
}

char	*ft_substr(char *str, size_t start, size_t len)
{
	char	*sub;
	size_t	len_str;
	size_t	len_sub;
	size_t	i;

	if (str == NULL)
		return (NULL);
	len_str = ft_strlen(str);
	if (start >= len_str)
		return (ft_strjoin("", ""));
	if (len_str - start > len)
		len_sub = len;
	else
		len_sub = len_str - start;
	sub = (char *)ft_calloc(len_sub + 1, sizeof(char));
	if (sub == NULL)
		return (NULL);
	i = 0;
	while (i < len_sub)
	{
		sub[i] = str[start + i];
		i++;
	}
	return (sub);
}

char	*ft_strjoin(char const *str1, char const *str2)
{
	char	*joined;
	size_t	len1;
	size_t	len2;
	size_t	i;

	if (str1 == NULL || str2 == NULL)
		return (NULL);
	len1 = ft_strlen(str1);
	len2 = ft_strlen(str2);
	joined = (char *)ft_calloc(len1 + len2 + 1, sizeof(char));
	if (joined == NULL)
		return (NULL);
	i = 0;
	while (i < len1)
	{
		joined[i] = str1[i];
		i++;
	}
	while (i < len1 + len2)
	{
		joined[i] = str2[i - len1];
		i++;
	}
	return (joined);
}

char	*ft_strchr(const char *str, int c)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if ((unsigned char)str[i] == (unsigned char)c)
			return ((char *)&str[i]);
		i++;
	}
	if (c == '\0')
		return ((char *)&str[i]);
	return (NULL);
}

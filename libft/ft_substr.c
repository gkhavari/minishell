/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:03:12 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:03:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_strcpy(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (dest_ptr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	len_s;
	size_t	len_sub;
	size_t	i;

	len_s = ft_strlen(s);
	if (start >= len_s)
		len_sub = 0;
	else if (len_s - start > len)
		len_sub = len;
	else
		len_sub = len_s - start;
	sub = (char *)malloc((len_sub + 1) * sizeof(char));
	if (sub == NULL)
		return (NULL);
	if (start >= len_s)
		return (ft_strcpy(sub, ""));
	i = 0;
	while (i < len_sub)
	{
		sub[i] = s[(size_t)start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

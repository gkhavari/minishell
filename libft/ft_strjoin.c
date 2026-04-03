/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:00:25 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:00:28 by gkhavari         ###   ########.fr       */
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

static char	*edge_case(const char *s1, const char *s2, char *joined)
{
	if (s1 == NULL && s2 == NULL)
		return (ft_strcpy(joined, ""));
	if (s1 == NULL)
		return (ft_strcpy(joined, s2));
	if (s2 == NULL)
		return (ft_strcpy(joined, s1));
	return (NULL);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*joined;
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	if (len1 + len2 + 1 < len1)
		return (NULL);
	joined = malloc(len1 + len2 + 1);
	if (joined == NULL)
		return (NULL);
	if (s1 == NULL || s2 == NULL)
		return (edge_case(s1, s2, joined));
	ft_strlcpy(joined, s1, len1 + 1);
	ft_strlcpy(joined + len1, s2, len2 + 1);
	return (joined);
}

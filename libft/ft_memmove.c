/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:57:45 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:57:48 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	*reverse_copy(char *dest, const char *src, size_t len)
{
	while (len > 0)
	{
		dest[len - 1] = src[len - 1];
		len--;
	}
	return ((void *)dest);
}

static void	*normal_copy(char *dest, const char *src, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
	return ((void *)dest);
}

void	*ft_memmove(void *dest, const void *src, size_t len)
{
	if (dest == NULL || src == NULL)
		return (NULL);
	if (len == 0)
		return (dest);
	if (dest > src)
		return (reverse_copy((char *)dest, (char *)src, len));
	else
		return (normal_copy((char *)dest, (char *)src, len));
}

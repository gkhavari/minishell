/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 16:00:00 by thanh-ng        #+#    #+#             */
/*   Updated: 2026/04/01 16:00:00 by thanh-ng       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_realloc(char *ptr, const size_t new_size)
{
	char	*res;
	size_t	old_size;
	size_t	copy_size;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (ptr == NULL)
		return (ft_calloc(new_size, sizeof(char)));
	res = ft_calloc(new_size, sizeof(char));
	if (!res)
		return (NULL);
	old_size = ft_strlen(ptr);
	if (old_size < new_size - 1)
		copy_size = old_size;
	else
		copy_size = new_size - 1;
	ft_memcpy(res, ptr, copy_size);
	free(ptr);
	return (res);
}

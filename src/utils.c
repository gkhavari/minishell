/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 22:47:25 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/25 22:47:27 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*ft_strcat(char *dest, const char *src)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (dest[i])
		i++;
	j = 0;
	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return (dest);
}

/**
 * ft_realloc - Reallocate a null-terminated string buffer.
 * @ptr:       Pointer to an existing null-terminated string, or NULL.
 * @new_size:  Size in bytes of the new buffer to allocate.
 *
 * This function re-allocates a memory block intended to hold a
 * null-terminated C-string. The contents of the original string are
 * copied into the newly allocated buffer, up to the maximum number of
 * bytes that can fit while preserving a terminating '\0'. If the
 * original string is longer than the new buffer, it is truncated.
 *
 * Behavior:
 *   • If @ptr is NULL, the function behaves like malloc() and returns
 *     a newly allocated empty string buffer of @new_size bytes.
 *
 *   • If @new_size is 0, the function frees @ptr and returns NULL.
 *
 *   • Otherwise, a new buffer of @new_size bytes is allocated, the
 *     existing string contents are copied into it, and the old buffer
 *     is freed. The result is always explicitly null-terminated.
 *
 * Return value:
 *   • Pointer to the newly allocated string buffer.
 *   • NULL if allocation fails or if @new_size == 0.
 *
 * Notes:
 *   • The input buffer must contain valid null-terminated text.
 *     Using this function on raw binary data produces undefined
 *     behavior because ft_strlen() is used internally.
 *
 *   • This function differs from the standard realloc() because it
 *     guarantees string null-termination and truncates safely if
 *     needed.
 */
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
	free (ptr);
	return (res);
}

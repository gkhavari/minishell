/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 22:47:25 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:18:03 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Concatenate `src` onto the end of `dest`.

 BEHAVIOR:
* Scans to the end of `dest` and copies `src` bytes including the NUL
* terminator into `dest`. Caller must ensure `dest` has enough space.

 PARAMETERS:
* char *dest: Destination buffer containing a NUL-terminated string.
* const char *src: Source string to append.

 RETURN:
* Pointer to `dest`.
*/
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
 DESCRIPTION:
* Simple realloc implementation for C-strings.

 BEHAVIOR:
* If `new_size` is zero frees `ptr` and returns NULL. If `ptr` is NULL
* allocates a zeroed buffer of `new_size`. Otherwise allocates a new
* buffer, copies up to `new_size-1` bytes, NUL-terminates and frees old
* buffer.

 PARAMETERS:
* char *ptr: Existing buffer or NULL.
* size_t new_size: New allocation size in bytes.

 RETURN:
* Pointer to the newly allocated buffer, or NULL on failure.
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
	free(ptr);
	return (res);
}

/**
 DESCRIPTION:
* Allocate zero-initialized memory for shell use, exiting on failure.

 BEHAVIOR:
* Wraps `ft_calloc`, prints an error, frees shell state and exits with
* `EXIT_FAILURE` if allocation fails. Returns the allocated memory on
* success.

 PARAMETERS:
* t_shell *shell: Shell runtime used for cleanup on failure.
* size_t nmemb: Number of elements.
* size_t size: Size of each element.

 RETURN:
* Pointer to zero-initialized memory; does not return on allocation failure.
*/
void	*msh_calloc(t_shell *shell, const size_t nmemb, const size_t size)
{
	char	*res;

	res = ft_calloc(nmemb, size);
	if (!res)
	{
		perror("minishell");
		free_all(shell);
		exit(EXIT_FAILURE);
	}
	return (res);
}

static void	ft_arrdup_cleanup(char **copy, size_t i)
{
	while (i > 0)
		free(copy[--i]);
	free(copy);
}

/**
 DESCRIPTION:
* Duplicate a NULL-terminated array of strings.

 BEHAVIOR:
* Allocates a new array and duplicates each string. On allocation
* failure frees partially-built copy and returns NULL.

 PARAMETERS:
* char **envp: NULL-terminated array of strings to duplicate.

 RETURN:
* Newly allocated duplicated array, or NULL on failure.
*/
char	**ft_arrdup(char **envp)
{
	char	**copy;
	size_t	i;
	size_t	count;

	if (!envp)
		return (NULL);
	count = 0;
	while (envp[count])
		count++;
	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		if (!copy[i])
			return (ft_arrdup_cleanup(copy, i), NULL);
		i++;
	}
	copy[count] = NULL;
	return (copy);
}

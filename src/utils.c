/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 22:47:25 by gkhavari          #+#    #+#             */
/*   Updated: 2026/01/16 16:28:17 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

char	*get_env_value(char **envp, const char *key)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(key);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, len) == 0 && envp[i][len] == '=')
			return (envp[i] + len + 1);
		i++;
	}
	return (NULL);
}

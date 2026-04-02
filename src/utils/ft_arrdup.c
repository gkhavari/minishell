/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 14:00:00 by thanh-ng        #+#    #+#               */
/*   Updated: 2026/04/01 16:00:00 by thanh-ng       ###   ########.fr         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_arrdup_cleanup(char **copy, size_t i)
{
	while (i > 0)
		free(copy[--i]);
	free(copy);
}

/** Deep copy of NULL-terminated string array; NULL on error. */
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

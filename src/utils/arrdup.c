/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arrdup.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:21:07 by gkhavari          #+#    #+#             */
/*   Updated: 2026/02/24 22:21:09 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
Helper function to free partially allocated array
*/
static void	free_partial(char **arr, size_t n)
{
	while (n > 0)
		free(arr[--n]);
	free(arr);
}

/*
Helper function to duplicate an array of strings
*/
static char	**duplicate_envp(char **envp, size_t count)
{
	char	**copy;
	size_t	i;

	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);

	i = 0;
	while (i < count)
	{
		copy[i] = strdup(envp[i]); // assuming ft_strdup is similar to strdup
		if (!copy[i])
		{
			free_partial(copy, i);
			return (NULL);
		}
		i++;
	}
	copy[count] = NULL;
	return (copy);
}

/*
Main function
*/
char	**ft_arrdup(char **envp)
{
	size_t	count;

	if (!envp)
		return (NULL);
	count = 0;
	while (envp[count])
		count++;
	return (duplicate_envp(envp, count));
}

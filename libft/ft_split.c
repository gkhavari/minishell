/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:59:33 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:59:36 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_words(const char *str, char c)
{
	size_t	words;
	size_t	index;

	words = 0;
	index = 0;
	if (str == NULL)
		return (0);
	while (str[index])
	{
		while (str[index] == c)
			index++;
		if (str[index] != '\0')
			words++;
		while (str[index] != '\0' && str[index] != c)
			index++;
	}
	return (words);
}

static void	free_all(char **dest, size_t count)
{
	while (count > 0)
		free(dest[--count]);
	free(dest);
}

static int	write_split(char ***dest, const char *str, const char c)
{
	size_t	word_len;
	size_t	j;

	j = 0;
	while (*str)
	{
		while (str && *str == c)
			str++;
		if (*str)
		{
			if (!ft_strchr(str, c))
				word_len = ft_strlen(str);
			else
				word_len = ft_strchr(str, c) - str;
			(*dest)[j++] = ft_substr(str, 0, word_len);
			if ((*dest)[j - 1] == NULL)
			{
				free_all(*dest, j - 1);
				return (0);
			}
			str = str + word_len;
		}
	}
	(*dest)[j] = NULL;
	return (1);
}

char	**ft_split(char const *str, char c)
{
	char	**dest;

	dest = (char **)malloc(sizeof(char *) * (count_words(str, c) + 1));
	if (dest == NULL)
		return (NULL);
	if (!write_split(&dest, str, c))
		return (NULL);
	return (dest);
}

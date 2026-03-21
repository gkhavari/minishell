/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:59:33 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 20:59:58 by thanh-ng         ###   ########.fr       */
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

static void	free_all(char **dest)
{
	size_t	i;

	i = 0;
	while (dest[i])
		free(dest[i++]);
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
				free_all(*dest);
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

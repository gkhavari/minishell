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
/*
#include <stdio.h>

typedef struct TestCase{
	char str[1024];
	char split;
}TestCase;

int main(void)
{
	TestCase test_cases[] = {
		{ .str = "          " , .split = ' ' },
		{ .str = "lorem ipsum dolor sit amet, consectetur adipiscing elit. \
		Sed non risus. Suspendisse" , .split = ' ' },
		{ .str = "   lorem   ipsum dolor     sit amet, consectetur   \
		adipiscing elit. Sed non risus. Suspendisse   " , .split = ' '},
		{ .str = "lorem ipsum dolor sit amet, consectetur adipiscing elit. \
		Sed non risus. Suspendisse lectus tortor, dignissim sit amet, \
		adipiscing nec, ultricies sed, dolor. Cras elementum ultricies \
		diam. Maecenas ligula massa, varius a, semper congue, euismod \
		non, mi." , .split = 'i' },
		{ .str = "lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed \
		non risus. Suspendisse lectus tortor, dignissim sit amet, \
		adipiscing nec, ultricies sed, dolor. Cras elementum ultricies diam. \
		Maecenas ligula massa, varius a, semper congue, euismod non, \
		mi." , .split = 'z'},
		{ .str = "" , .split = 'z' }
	};

	size_t num_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	size_t i = 0;
	while (i < num_of_tests)
	{
		printf("Start of test %zu\n", i+1);
		char **arr = ft_split(test_cases[i].str, test_cases[i].split);
		size_t j = 0;
		printf("String: %s\n", test_cases[i].str);
		printf("Delimiter: %c\n", test_cases[i].split);
		size_t words = count_words(test_cases[i].str, test_cases[i].split);
		if (words == 0)
			printf("Split: %s\n", arr[0]);
		while (j < words)
		{
			printf("Split: %s\n", arr[j]);
			j++;
		}
		printf("\n");
		i++;
	}
}*/

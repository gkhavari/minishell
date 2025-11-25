/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:02:42 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:02:44 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_in_set(const char c, char const *set)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*trimmed;
	size_t	start;
	size_t	end;
	size_t	i;

	start = 0;
	while (s1[start] && is_in_set(s1[start], set))
		start++;
	end = ft_strlen(s1);
	while (end > start && is_in_set(s1[end - 1], set))
		end--;
	trimmed = (char *)malloc((end - start + 1) * sizeof(char));
	if (trimmed == NULL)
		return (NULL);
	i = 0;
	while (start < end)
		trimmed[i++] = s1[start++];
	trimmed[i] = '\0';
	return (trimmed);
}
/*
#include <stdio.h>

typedef struct TestCase{
	char str[1024];
	char set[1024];
} TestCase;

int main(void)
{
	TestCase test_cases[] = {
		{ .str = "lorem \n ipsum \t dolor \n sit \t amet", .set = " "},
		{ .str = "lorem ipsum dolor sit amet", .set = "te"},
		{ .str = " lorem ipsum dolor sit amet", .set = "l "},
		{ .str = "lorem ipsum dolor sit amet", .set = "tel"},
		{ .str = "          ", .set = " "}
	};
	size_t num_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	size_t i = 0;
	while (i < num_of_tests)
	{
		printf("String:       %s\n", test_cases[i].str);
		printf("Delimiter:    %s\n", test_cases[i].set);
		printf("Output:       %s\n\n", ft_strtrim(test_cases[i].str, \
		test_cases[i].set));
		i++;
	}
}*/

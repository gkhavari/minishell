/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:02:07 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:02:10 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (haystack == NULL || needle == NULL)
		return (NULL);
	if (needle[0] == '\0')
		return ((char *)haystack);
	while (haystack[i] && i < len)
	{
		j = 0;
		while (haystack[i + j] == needle[j] && haystack[i + j] && i + j < len)
		{
			if (needle [j + 1] == '\0')
				return ((char *)&haystack[i]);
			j++;
		}
		i++;
	}
	return (NULL);
}
/*
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct TestCase {
	char haystack[1024];
	char needle[1024];
	size_t size;
} TestCase;

int main(void)
{
	TestCase test_cases[] = {
		{ .haystack = "lorem ipsum dolor sit amet", 
		.needle = "lorem", .size = 15},
		{ .haystack = "lorem ipsum dolor sit amet", 
		.needle = "ipsum", .size = 15},
		{ .haystack = "lorem ipsum dolor sit lorem",
		 .needle = "ipsum", .size = 35},
		{ .haystack = "lorem ipsum dolor sit amet", .needle = "", .size = 10},
		{ .haystack = "lorem ipsum dolor sit amet", 
		.needle = "ipsumm", .size = 30},
		{ .haystack = "lorem ipsum dolor sit amet", .needle = "dol", .size = 30},
		{ .haystack = "lorem ipsum dolor sit amet", 
		.needle = "consectetur", .size = 30},
		{ .haystack = "lorem ipsum dolor sit amet", .needle = "sit", .size = 10},
		{ .haystack = "lorem ipsum dolor sit amet", 
		.needle = "dolor", .size = 15},
		{ .haystack = "lorem ipsum dolor sit amet", .needle = "dolor", .size = 0}
	};
	
	size_t numb_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	size_t i = 0;
	while (i < numb_of_tests)
	{
		printf("--- Test no. %zu ---\n", i + 1);
		printf("Haystack: %s\n", test_cases[i].haystack);
		printf("Needle:   %s\n", test_cases[i].needle);
		printf("Size:     %zu\n", test_cases[i].size);
		printf("Result:   %s\n\n", ft_strnstr(test_cases[i].haystack, 
		test_cases[i].needle, test_cases[i].size));
		i++;
	}
}*/

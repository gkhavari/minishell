/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:56:52 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:56:54 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*str;
	size_t				i;

	if (s == NULL || n == 0)
		return (NULL);
	str = (const unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (str[i] == (unsigned char)c)
			return ((void *)&str[i]);
		i++;
	}
	return (NULL);
}
/*
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct TestCase {
	char str[1024];
	int to_find;
	int size;
} TestCase;

void	test_strchr(TestCase *test_case)
{
		if (ft_memchr(test_case->str, test_case->to_find, test_case->size)
		== memchr(test_case->str, test_case->to_find, test_case->size))
			printf("Test passed!\n");
		else
			printf("Test failed!\n");
}

int main(void)
{
	TestCase test_cases[] = {
		{ .str = "Hello, World!", .to_find = 'H' , .size = 4},
		{ .str = "Hello, World!", .to_find = 'l' , .size = 5},
		{ .str = "Hello, World!", .to_find = '!' , .size = 3},
		{ .str = "Hello, World!", .to_find = '!' , .size = 0},
		{ .str = "Hello, World!", .to_find = 'z' , .size = 13},
		{ .str = "", .to_find = 'a' , .size = 6},
		{ .str = "42", .to_find = '\0' , .size = 3},
		{ .str = "a", .to_find = 'a' , .size = 3},
		{ .str = "a", .to_find = 'A' , .size = 3},
		{ .str = "aaaaaaaaaa", .to_find = 'A' , .size = 15},
		{ .str = "Hello\nworld", .to_find = '\n' , .size = 30}
	};
	
	int numb_of_cases = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;
	while (i < numb_of_cases)
	{
		test_strchr(&test_cases[i]);
		i++;
	}
}*/

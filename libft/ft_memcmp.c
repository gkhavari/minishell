/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:57:05 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:57:07 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*str1;
	const unsigned char	*str2;
	size_t				i;

	str1 = (const unsigned char *)s1;
	str2 = (const unsigned char *)s2;
	if (s1 == NULL && s2 == NULL)
		return (0);
	if (s1 == NULL)
		return (-((unsigned char *)s2)[0]);
	if (s2 == NULL)
		return (((unsigned char *)s1)[0]);
	i = 0;
	while (i < n)
	{
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
		i++;
	}
	return (0);
}
/*
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct TestCase {
	char str1[1024];
	char str2[1024];
	size_t size;
} TestCase;

void	test_memcmp(TestCase *test_case)
{
		if (ft_memcmp(test_case->str1, test_case->str2, test_case->size) == 
		memcmp(test_case->str1, test_case->str2, test_case->size))
			printf("Test passed!\n");
		else
			printf("Test failed!\n");
}

int main(void)
{
	TestCase test_cases[] = {
		{ .str1 = "Hello, World!", .str2 = "Hello, World?" , .size = 15},
		{ .str1 = "Hello, World!", .str2 = "Hello, World?" , .size = 10},
		{ .str1 = "Hello, World!", .str2 = "Hello, World?" , .size = 0},
		{ .str1 = "", .str2 = "hellohello" , .size = 6},
		{ .str1 = "42", .str2 = "" , .size = 3},
		{ .str1 = "", .str2 = "" , .size = 3}
	};
	
	int numb_of_cases = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;
	while (i < numb_of_cases)
	{
		test_memcmp(&test_cases[i]);
		i++;
	}
}*/

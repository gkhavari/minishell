/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:57:22 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:57:25 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t				i;
	unsigned char		*d;
	const unsigned char	*s;

	if (dest == NULL || src == NULL)
		return (NULL);
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return ((void *)dest);
}
/*
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 1024

typedef struct TestCase {
	unsigned char src[BUFFER_SIZE];
	int size;
} TestCase;

void print_str(const unsigned char* str, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%d ", str[i]);
	printf("\n");
}

void test_ft_memcpy(TestCase *testCase)
{
	unsigned char dest_custom[BUFFER_SIZE] = {0};
	unsigned char dest_std[BUFFER_SIZE] = {0};

	ft_memcpy(dest_custom, testCase->src, testCase->size);
	memcpy(dest_std, testCase->src, testCase->size);

	printf("Source:      ");
	print_str(testCase->src, testCase->size);
	printf("ft_memcpy:   ");
	print_str(dest_custom, testCase->size);
	printf("std memcpy:  ");
	print_str(dest_std, testCase->size);

	if (memcmp(dest_custom, dest_std, testCase->size) == 0)
		printf("Test passed\n");
	else
		printf("Test failed\n");
	printf("--------------\n");	
}

int main(void)
{
	TestCase test_cases[] = {
		{ .src = "zyxwvutsrqponmlkjihgfedcba", .size = 14 },
		{ .src = "zyxwvutst", .size = 0 },
		{ .src = "zy\0xw\0vu\0\0tsr", .size = 11 },
		{ .src = "hello world", .size = 0 }
	};
	
	int numb_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;
	while (i < numb_of_tests)
	{
		test_ft_memcpy(&test_cases[i]);
		i++;
	}
}*/

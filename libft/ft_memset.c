/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:58:03 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:58:06 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *str, int c, size_t n)
{
	size_t			i;
	unsigned char	*ptr;

	if (str == NULL || n == 0)
		return (str);
	ptr = (unsigned char *)str;
	i = 0;
	while (i < n)
	{
		ptr[i] = (unsigned char)c;
		i++;
	}
	return (str);
}
/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

typedef struct TestCase {
	unsigned char buffer1[BUFFER_SIZE];
	unsigned char buffer2[BUFFER_SIZE];
	int size;
	unsigned char value;
} TestCase;

void print_buffer(const unsigned char* buffer, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%d ", buffer[i]);
	printf("\n");
}

void test_ft_memset(TestCase* testCase)
{
	memset(testCase->buffer1, testCase->value, testCase->size);
	ft_memset(testCase->buffer2, testCase->value, testCase->size);
	print_buffer(testCase->buffer1, testCase->size);
	print_buffer(testCase->buffer2, testCase->size);
	if (memcmp(testCase->buffer1, testCase->buffer2, testCase->size) == 0)
		printf("Test Passed!\n\n");
	else
		printf("Test Failed!\n\n");
}

int main(void)
{
	TestCase test_cases[] = {
		{ .size = 10, .value = 0 },
		{ .size = 10, .value = 0xAB },
		{ .size = 5, .value = 'A' },
		{ .size = 0, .value = 0xFF }
	};
	
	int numb_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;

	while (i < numb_of_tests)
	{
		test_ft_memset(&test_cases[i]);
		i++;
	}
}*/

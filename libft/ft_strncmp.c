/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:01:46 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:01:48 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(char *s1, char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while (i < n - 1 && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
/*
#include <string.h>
#include <stdio.h>

typedef struct TestCase {
	char *s1;
	char *s2;
	size_t size;
} TestCase;

int main (void)
{
	TestCase test_cases[] = {
		{ .s1 ="Hello", .s2 ="Hello", .size = 5},
		{ .s1 ="Hello", .s2 ="Hell", .size = 5},
		{ .s1 ="Hello", .s2 ="Hell", .size = 4},
		{ .s1 ="Hello", .s2 ="Helloo", .size = 5},
		{ .s1 ="Hello", .s2 ="World", .size = 3},
		{ .s1 ="Hello", .s2 ="hello", .size = 5},
		{ .s1 ="", .s2 ="", .size = 1},
		{ .s1 ="a", .s2 ="b", .size = 1},
		{ .s1 ="abc", .s2 ="abc", .size = 0},
		{ .s1 ="abc", .s2 ="abcd", .size = 4},
	};
	int num_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;
	while (i < num_of_tests)
	{
		char *s1 = test_cases[i].s1;
		char *s2 = test_cases[i].s2;
		size_t n = test_cases[i].size;
		
		int my_result = ft_strncmp(s1, s2, n);
		int lib_result = strncmp(s1, s2, n);
		
		if (my_result != lib_result)
			printf("Test %d failed! ft_strncmp returned %d, 
			strncmp returned %d.\n", i + 1, my_result, lib_result);
		else
			printf("Test %d passed. ft_strncmp returned %d, 
			strncmp returned %d.\n", i + 1, my_result, lib_result);
		i++;
	}
}*/

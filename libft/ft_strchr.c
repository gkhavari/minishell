/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:59:52 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:59:54 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;

	if (s == NULL)
		return (NULL);
	i = 0;
	while (s[i])
	{
		if ((unsigned char)s[i] == (unsigned char)c)
			return ((char *)&s[i]);
		i++;
	}
	if (c == '\0')
		return ((char *)&s[i]);
	return (NULL);
}
/*
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct TestCase {
	char str[1024];
	int to_find;
} TestCase;

void	test_strchr(TestCase *test_case)
{
		if (ft_strchr(test_case->str, test_case->to_find) == 
		strchr(test_case->str, test_case->to_find))
			printf("Test passed!\n");
		else
			printf("Test failed!\n");
}

int main(void)
{
	TestCase test_cases[] = {
		{ .str = "Hello, World!", .to_find = 'H' },
		{ .str = "Hello, World!", .to_find = 'l' },
		{ .str = "Hello, World!", .to_find = '!' },
		{ .str = "Hello, World!", .to_find = 'z' },
		{ .str = "", .to_find = 'a' },
		{ .str = "42", .to_find = '\0' },
		{ .str = "a", .to_find = 'a' },
		{ .str = "a", .to_find = 'A' },
		{ .str = "aaaaaaaaaa", .to_find = 'A' },
		{ .str = "Hello\nworld", .to_find = '\n' }
	};
	
	int numb_of_cases = sizeof(test_cases) / sizeof(test_cases[0]);
	int i = 0;
	while (i < numb_of_cases)
	{
		test_strchr(&test_cases[i]);
		i++;
	}
}*/

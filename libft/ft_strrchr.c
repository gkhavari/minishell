/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:02:25 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:02:27 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int	len;

	if (s == NULL)
		return (NULL);
	len = ft_strlen(s);
	while (len >= 0)
	{
		if (s[len] == (unsigned char)c)
			return ((char *)&s[len]);
		len--;
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
} TestCase;

void	test_strrchr(TestCase *test_case)
{
		if (ft_strrchr(test_case->str, test_case->to_find) 
		== strrchr(test_case->str, test_case->to_find))
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
		test_strrchr(&test_cases[i]);
		i++;
	}
}*/

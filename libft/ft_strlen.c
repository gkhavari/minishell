/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:01:06 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:01:08 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	if (str == NULL)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}
/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	char *test5 = (char *)calloc(1000, sizeof(char));
	char test6[3] = {'4', '2', '!'};
	
	char *tests[] = {
		"hello42",
		"",
		"a",
		"hello\nworld",
		"hello\0world",
		test5,
		test6,
		};
	size_t num_of_tests = sizeof(tests) / sizeof(tests[0]);
	size_t i = 0;
	while (i < num_of_tests)
	{
		if (ft_strlen(tests[i]) != strlen(tests[i]))
		{
			printf("ft_strlen failed on test %zu\n", i);
			printf("returned %zu, expected %lu\n", 
			ft_strlen(tests[i]), strlen(tests[i]));
		}
		i++;
	}
	printf("Finished testing ft_strlen\n");	
	free(test5);
}*/

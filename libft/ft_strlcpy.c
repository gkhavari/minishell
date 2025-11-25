/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:00:53 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:00:55 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size == 0)
		return (ft_strlen(src));
	if (src == NULL)
		return (0);
	if (dest == NULL)
		return (ft_strlen(src));
	while (i < size - 1 && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	if (size > 0)
		dest[i] = '\0';
	return (ft_strlen(src));
}
/*
#include <stdio.h>
#include <string.h>

void test_strlcpy(const char *src, size_t size)
{
	char dest[100];

	size_t result = ft_strlcpy(dest, src, size);

	printf("Source: \"%s\"\n", src);
	printf("Size: %zu\n", size);
	printf("Custom ft_strlcpy: \"%s\" (Returned: %zu)\n", dest, result);
}

int main()
{
	// Test cases
	test_strlcpy("Hello, World!", 20);
	test_strlcpy("Hello", 10);
	test_strlcpy("4242", 3);
	test_strlcpy("Another test with a long string to see buffer size", 10);
	test_strlcpy("", 5);
	test_strlcpy("Test", 0);
	test_strlcpy(NULL, 10);
	
	return 0;
}*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:57:45 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:57:48 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	*reverse_copy(char *dest, const char *src, size_t len)
{
	while (len > 0)
	{
		dest[len - 1] = src[len - 1];
		len--;
	}
	return ((void *)dest);
}

static void	*normal_copy(char *dest, const char *src, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		dest[i] = src[i];
		i++;
	}
	return ((void *)dest);
}

void	*ft_memmove(void *dest, const void *src, size_t len)
{
	if (dest == NULL || src == NULL)
		return (NULL);
	if (len == 0)
		return (dest);
	if (dest > src)
		return (reverse_copy((char *)dest, (char *)src, len));
	else
		return (normal_copy((char *)dest, (char *)src, len));
}
/*
#define BUFFER_SIZE 1024
#include <string.h>
#include <stdio.h>

void print_str(const unsigned char* str, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%d ", str[i]);
	printf("\n");
}

void test_ft_memmove(unsigned char *src, unsigned char *dest, 
size_t size, const char *description)
{
	printf("Test: %s\n", description);

	unsigned char expected[BUFFER_SIZE] = {0};

	// Set initial values (in case of overlap)
	if (src && dest && src != expected && dest != expected)
	{
		memcpy(expected, dest, BUFFER_SIZE);
		memmove(expected, src, size);
	}

	// Perform ft_memmove
	unsigned char *result = ft_memmove(dest, src, size);

	if (src != NULL && dest != NULL)
	{
		if (memcmp(dest, expected, size) == 0)
			printf("Passed\n");
		else
		{
			printf("Failed (incorrect move)\n");
			printf("Expected: ");
			print_str(expected, size);
			printf("Got     : ");
			print_str(dest, size);
		}
	}
	else
	{
		if (result == NULL)
			printf("Passed (NULL handled correctly)\n");
		else
			printf("Failed (expected NULL return)\n");
	}

	printf("--------------\n");
}

int main(void)
{
	unsigned char buf1[BUFFER_SIZE] = "abcdefghijklmnopqrstuvwxyz";
	unsigned char binary_src[] = {1, 2, 0, 4, 0, 5, 6};
	unsigned char temp[BUFFER_SIZE] = {0};

	// Normal move
	test_ft_memmove(buf1, temp, 10, "Normal memmove");

	// Overlapping forward
	memcpy(buf1, "1234567890", 10); // Reset
	test_ft_memmove(buf1, buf1 + 2, 8, "Overlapping forward (dest > src)");

	// Overlapping backward
	memcpy(buf1, "1234567890", 10); // Reset again
	test_ft_memmove(buf1 + 2, buf1, 8, "Overlapping backward (src > dest)");

	// Binary data
	test_ft_memmove(binary_src, temp, sizeof(binary_src), 
	"Binary data with null bytes");

	// NULL source
	test_ft_memmove(NULL, temp, 5, "NULL source");

	// NULL destination
	test_ft_memmove(binary_src, NULL, 5, "NULL destination");

	// Zero size
	test_ft_memmove(buf1, temp, 0, "Zero-length memmove");
}*/

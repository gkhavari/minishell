/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:00:38 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:00:41 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	len_dest;
	size_t	len_src;
	size_t	i;

	len_src = ft_strlen(src);
	len_dest = 0;
	while (len_dest < size && dest[len_dest])
		len_dest++;
	if (len_dest == size)
		return (size + len_src);
	i = 0;
	while ((len_dest + i < size - 1) && src[i])
	{
		dest[len_dest + i] = src[i];
		i++;
	}
	if (len_dest + i < size)
		dest[len_dest + i] = '\0';
	return (len_dest + len_src);
}
/*
#include <stdio.h>
#include <string.h>

void test_strlcat(const char *src, size_t size)
{
    char dest[100];
    strcpy(dest, "Hello ");
    
    size_t result_ft = ft_strlcat(dest, src, size);
    
    printf("Testing ft_strlcat\n");
    printf("Source: \"%s\"\n", src);
    printf("Size: %zu\n", size);
    printf("ft_strlcat result: %zu, dest: \"%s\"\n", result_ft, dest);
    printf("\n");
}

int main()
{
    test_strlcat("world!", 20);
    test_strlcat("world!", 5);
    test_strlcat("world!", 0);
    test_strlcat("some longer string that will exceed buffer", 15);
    test_strlcat("", 10);
    
    return 0;
}*/

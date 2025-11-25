/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:03:12 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:03:15 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_strcpy(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (dest_ptr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	len_s;
	size_t	len_sub;
	size_t	i;

	len_s = ft_strlen(s);
	if (len_s - start > len)
		len_sub = len;
	else
		len_sub = len_s - start;
	sub = (char *)malloc((len_sub + 1) * sizeof(char));
	if (sub == NULL)
		return (NULL);
	if (start >= len_s)
		return (ft_strcpy(sub, ""));
	i = 0;
	while (i < len_sub)
	{
		sub[i] = s[(size_t)start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}
/*
#include <stdio.h>
typedef struct TestCase {
	char str[1024];
	int start;
	int len; 
	} TestCase;

int main(void)
{
	TestCase test_cases[] = {
		{.str = "Lorem ipsum dolor sit amet" , .start = 0, .len = 26},
		{.str = "Lorem ipsum dolor sit amet" , .start = 0, .len = 30},
		{.str = "Lorem ipsum dolor sit amet" , .start = 5, .len = 20},
		{.str = "Lorem ipsum dolor sit amet" , .start = 5, .len = 5},
		{.str = "Lorem ipsum dolor sit amet" , .start = 5, .len = 30},
		{.str = "Lorem ipsum dolor sit amet" , .start = 10, .len = 5},
		{.str = "Lorem ipsum dolor sit amet" , .start = 20, .len = 10},
		{.str = "Lorem ipsum dolor sit amet" , .start = 10, .len = 0},
		{.str = "Lorem ipsum dolor sit amet" , .start = 30, .len = 5},
		{.str = "Lorem ipsum dolor sit amet" , .start = 30, .len = 5},
		{.str = "Lorem ipsum dolor sit amet" , .start = 0, .len = 0}
	};
	size_t numb_of_tests = sizeof(test_cases) / sizeof (test_cases[0]);
	
	size_t i = 0;
	while (i < numb_of_tests)
	{
		printf("Input string:     %s\n", test_cases[i].str);
		printf("Start index:      %d\n", test_cases[i].start);
		printf("Output length:    %d\n", test_cases[i].len);
		printf("Output string:    %s\n\n", ft_substr(test_cases[i].str, \
		 test_cases[i].start, test_cases[i].len));
		i++;
	}
}*/

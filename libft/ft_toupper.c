/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:03:49 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:03:51 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if ('a' <= c && c <= 'z')
		c = c - 32;
	return (c);
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (ft_toupper(i) != toupper(i))
		{
			printf("ft_toupper: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_toupper(i), toupper(i));
			return (1);
		}
		i++;
	}
	printf("ft_toupper: ok\n");
	return (0);
}*/

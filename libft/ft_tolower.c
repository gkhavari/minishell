/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:03:31 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:03:34 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if ('A' <= c && c <= 'Z')
		c = c + 32;
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
		if (ft_tolower(i) != tolower(i))
		{
			printf("ft_tolower: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_tolower(i), tolower(i));
			return (1);
		}
		i++;
	}
	printf("ft_tolower: ok\n");
	return (0);
}*/

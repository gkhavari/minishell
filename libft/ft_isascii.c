/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:52:44 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:52:47 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	return (0 <= c && c <= 127);
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (!!ft_isascii(i) != !!isascii(i))
		{
			printf("ft_isascii: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_isascii(i), isascii(i));
			return (1);
		}
		i++;
	}
	printf("ft_isascii: ok\n");
	return (0);
}*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:53:09 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:53:11 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isprint(int c)
{
	return (' ' <= c && c <= '~');
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (!!ft_isprint(i) != !!isprint(i))
		{
			printf("ft_isprint: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_isprint(i), isprint(i));
			return (1);
		}
		i++;
	}
	printf("ft_isprint: ok\n");
	return (0);
}*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:52:25 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:52:33 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (!!ft_isalpha(i) != !!isalpha(i))
		{
			printf("ft_isalpha: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_isalpha(i), isalpha(i));
			return (1);
		}
		i++;
	}
	printf("ft_isalpha: ok\n");
	return (0);
}*/

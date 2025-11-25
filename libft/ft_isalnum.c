/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:52:10 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:52:14 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (!!ft_isalnum(i) != !!isalnum(i))
		{
			printf("ft_isalnum: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_isalnum(i), isalnum(i));
			return (1);
		}
		i++;
	}
	printf("ft_isalnum: ok\n");
	return (0);
}*/

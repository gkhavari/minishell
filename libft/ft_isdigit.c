/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:52:57 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:52:59 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	return ('0' <= c && c <= '9');
}
/*
#include <stdio.h>
#include <ctype.h>
int	main(void)
{
	int i = 0;
	while (i < 127)
	{
		if (!!ft_isdigit(i) != !!isdigit(i))
		{
			printf("ft_isdigit: failed on %d\n", i);
			printf("my function gives %d, expected %d\n", 
			ft_isdigit(i), isdigit(i));
			return (1);
		}
		i++;
	}
	printf("ft_isdigit: ok\n");
	return (0);
}
*/

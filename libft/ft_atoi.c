/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:51:07 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 13:51:14 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	edge_case(int sign)
{
	if (sign == 1)
		return (2147483647);
	else
		return (-2147483648);
}

int	ft_atoi(char *str)
{
	size_t	i;
	int		sign;
	int		number;

	i = 0;
	sign = 1;
	number = 0;
	if (str == NULL || str[i] == '\0')
		return (0);
	while ((8 <= str[i] && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -sign;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (number > (2147483647 - (str[i] - '0')) / 10)
			return (edge_case(sign));
		number = number * 10 + (str[i] - '0');
		i++;
	}
	return (sign * number);
}
/*
#include <stdio.h>

int main(void)
{
	printf("%d\n", (ft_atoi("0")));
	printf("%d\n", (ft_atoi("546:5")));
	printf("%d\n", (ft_atoi("-4886")));
	printf("%d\n", (ft_atoi("+548")));
	printf("%d\n", (ft_atoi("054854")));
	printf("%d\n", (ft_atoi("000074")));
	printf("%d\n", (ft_atoi("+-54")));
	printf("%d\n", (ft_atoi("-+48")));
	printf("%d\n", (ft_atoi("--47")));
	printf("%d\n", (ft_atoi("++47")));
	printf("%d\n", (ft_atoi("+47+5")));
	printf("%d\n", (ft_atoi("-47-5")));
	printf("%d\n", (ft_atoi("\e475")));//
	printf("%d\n", (ft_atoi("\t\n\r\v\f  469 \n")));
	printf("%d\n", (ft_atoi("-2147483648")));
	printf("%d\n", (ft_atoi("2147483647")));
	printf("%d\n", (ft_atoi("\t\n\r\v\fd469 \n")));//
	printf("%d\n", (ft_atoi("\n\n\n  -46\b9 \n5d6")));
	printf("%d\n", (ft_atoi("")));
}*/

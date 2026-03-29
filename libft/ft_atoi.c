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

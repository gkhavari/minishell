/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_unbr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:49:59 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_unsigned_fd(int fd, unsigned int n)
{
	char	buf[20];
	int		i;
	int		count;

	if (!n)
		return (print_chr_fd(fd, '0'));
	i = 0;
	count = 0;
	while (n)
	{
		buf[i++] = (n % 10) + '0';
		n /= 10;
	}
	while (i--)
	{
		if (print_chr_fd(fd, buf[i]) < 0)
			return (-1);
		count++;
	}
	return (count);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_hex.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:52:13 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	put_hex_fd(int fd, unsigned int num, char c)
{
	char	buf[20];
	int		i;
	int		count;
	int		digit;

	if (!num)
		return (print_chr_fd(fd, '0'));
	i = 0;
	count = 0;
	while (num)
	{
		digit = num % 16;
		if (digit < 10)
			buf[i++] = digit + '0';
		else
			buf[i++] = digit - 33 + c;
		num /= 16;
	}
	while (i--)
	{
		if (print_chr_fd(fd, buf[i]) < 0)
			return (-1);
		count++;
	}
	return (count);
}

int	print_hex_fd(int fd, unsigned int n, char c)
{
	return (put_hex_fd(fd, n, c));
}

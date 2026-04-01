/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_ptr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:51:58 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	ft_print_adr_fd(int fd, unsigned long long n)
{
	if (n >= 16)
	{
		ft_print_adr_fd(fd, n / 16);
		ft_print_adr_fd(fd, n % 16);
	}
	else
	{
		if (n < 10)
			print_chr_fd(fd, n + '0');
		else if (n >= 10)
			print_chr_fd(fd, (n - 10) + 'a');
	}
}

static int	ft_numlen(unsigned long long n, int base)
{
	int	len;

	len = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= base;
		len++;
	}
	return (len);
}

int	print_pointer_fd(int fd, unsigned long long n)
{
	int	count;

	count = 0;
	if (n == 0)
	{
		count += print_str_fd(fd, "(nil)");
		return (count);
	}
	count += print_str_fd(fd, "0x");
	ft_print_adr_fd(fd, n);
	count += ft_numlen(n, 16);
	return (count);
}

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

static int	ft_print_adr_fd(int fd, unsigned long long n)
{
	int	count;
	int	ret;

	count = 0;
	if (n >= 16)
	{
		ret = ft_print_adr_fd(fd, n / 16);
		if (ret < 0)
			return (-1);
		count += ret;
		ret = ft_print_adr_fd(fd, n % 16);
		if (ret < 0)
			return (-1);
		count += ret;
	}
	else
	{
		if (n < 10)
			count = print_chr_fd(fd, n + '0');
		else
			count = print_chr_fd(fd, (n - 10) + 'a');
		if (count < 0)
			return (-1);
	}
	return (count);
}

int	print_pointer_fd(int fd, unsigned long long n)
{
	int	count;
	int	ret;

	count = 0;
	if (n == 0)
		return (print_str_fd(fd, "(nil)"));
	ret = print_str_fd(fd, "0x");
	if (ret < 0)
		return (-1);
	count += ret;
	ret = ft_print_adr_fd(fd, n);
	if (ret < 0)
		return (-1);
	return (count + ret);
}

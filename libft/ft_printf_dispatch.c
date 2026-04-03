/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_dispatch.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	try_basic(int fd, char f, va_list ap)
{
	if (f == '%')
		return (print_chr_fd(fd, '%'));
	if (f == 'c')
		return (print_chr_fd(fd, (char)va_arg(ap, int)));
	if (f == 's')
		return (print_str_fd(fd, va_arg(ap, char *)));
	return (PF_CONV_UNHANDLED);
}

static int	try_nums(int fd, char f, va_list ap)
{
	if (f == 'd' || f == 'i')
		return (print_nbr_fd(fd, va_arg(ap, int)));
	if (f == 'x' || f == 'X')
		return (print_hex_fd(fd, va_arg(ap, unsigned int), f));
	if (f == 'u')
		return (print_unsigned_fd(fd, va_arg(ap, unsigned int)));
	if (f == 'p')
		return (print_pointer_fd(fd,
				(unsigned long long)va_arg(ap, void *)));
	return (PF_CONV_UNHANDLED);
}

int	dispatch_printf_conv(int fd, char format, va_list ap)
{
	int	wr1;
	int	wr2;
	int	res;

	res = try_basic(fd, format, ap);
	if (res != PF_CONV_UNHANDLED)
		return (res);
	res = try_nums(fd, format, ap);
	if (res != PF_CONV_UNHANDLED)
		return (res);
	wr1 = print_chr_fd(fd, '%');
	if (wr1 < 0)
		return (-1);
	wr2 = print_chr_fd(fd, format);
	if (wr2 < 0)
		return (-1);
	return (wr1 + wr2);
}

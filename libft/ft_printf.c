/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:49:07 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	check_conversion_fd(int fd, const char format, va_list ap)
{
	if (format == '%')
		return (print_chr_fd(fd, '%'));
	if (format == 'c')
		return (print_chr_fd(fd, (char)va_arg(ap, int)));
	if (format == 's')
		return (print_str_fd(fd, va_arg(ap, char *)));
	if (format == 'd' || format == 'i')
		return (print_nbr_fd(fd, va_arg(ap, int)));
	if (format == 'x' || format == 'X')
		return (print_hex_fd(fd, va_arg(ap, unsigned int), format));
	if (format == 'u')
		return (print_unsigned_fd(fd, va_arg(ap, unsigned int)));
	if (format == 'p')
		return (print_pointer_fd(fd,
				(unsigned long long)va_arg(ap, void *)));
	return (print_chr_fd(fd, '%') + print_chr_fd(fd, format));
}

static int	ft_vdprintf(int fd, const char *format, va_list ap)
{
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (format[i])
	{
		if (format[i] == '%' && !format[i + 1])
			return (-1);
		else if (format[i] == '%' && format[i + 1])
			count += check_conversion_fd(fd, format[++i], ap);
		else
			count += print_chr_fd(fd, format[i]);
		i++;
	}
	return (count);
}

int	ft_dprintf(int fd, const char *format, ...)
{
	va_list	ap;
	int		n;

	if (!format)
		return (-1);
	va_start(ap, format);
	n = ft_vdprintf(fd, format, ap);
	va_end(ap);
	return (n);
}

int	ft_printf(const char *format, ...)
{
	va_list	ap;
	int		n;

	if (!format)
		return (-1);
	va_start(ap, format);
	n = ft_vdprintf(1, format, ap);
	va_end(ap);
	return (n);
}

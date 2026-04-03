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

static int	handle_fmt_char(int fd, const char *f, int *i, va_list *ap)
{
	int	ret;

	if (f[*i] == '%' && !f[*i + 1])
		return (-1);
	if (f[*i] == '%' && f[*i + 1])
	{
		(*i)++;
		ret = dispatch_printf_conv(fd, f[*i], ap);
	}
	else
		ret = print_chr_fd(fd, f[*i]);
	return (ret);
}

static int	ft_vdprintf(int fd, const char *format, va_list ap)
{
	int		count;
	int		i;
	int		ret;

	count = 0;
	i = 0;
	while (format[i])
	{
		ret = handle_fmt_char(fd, format, &i, &ap);
		if (ret < 0)
			return (-1);
		count += ret;
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

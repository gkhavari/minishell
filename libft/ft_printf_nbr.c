/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_nbr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:52:07 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_nbr_fd(int fd, int n)
{
	int	count;
	int	ret;

	count = 0;
	if (n == -2147483648)
		return (print_str_fd(fd, "-2147483648"));
	if (n < 0)
	{
		if (print_chr_fd(fd, '-') < 0)
			return (-1);
		count++;
		n = -n;
	}
	ret = print_unsigned_fd(fd, (unsigned int)n);
	if (ret < 0)
		return (-1);
	return (count + ret);
}

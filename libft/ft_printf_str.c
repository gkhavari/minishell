/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_str.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/17 18:49:08 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print_str_fd(int fd, char *s)
{
	int	count;
	int	ret;

	if (!s)
		s = "(null)";
	count = 0;
	while (*s)
	{
		ret = print_chr_fd(fd, *s);
		if (ret < 0)
			return (-1);
		count += ret;
		s++;
	}
	return (count);
}

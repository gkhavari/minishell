/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_write.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 00:00:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <errno.h>

/**
 * Write exactly n bytes (retries on EINTR; -1 on error or spurious 0 return).
 */
int	ft_write_all(int fd, const void *buf, size_t n)
{
	const char	*p;
	size_t		done;
	ssize_t		w;

	p = (const char *)buf;
	done = 0;
	while (done < n)
	{
		w = write(fd, p + done, n - done);
		if (w < 0)
		{
			if (errno == EINTR)
				continue ;
			return (-1);
		}
		if (w == 0)
			return (-1);
		done += (size_t)w;
	}
	return (0);
}

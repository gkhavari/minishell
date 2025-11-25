/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:53:29 by gkhavari          #+#    #+#             */
/*   Updated: 2025/04/22 14:05:27 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_strcpy(char *dest, const char *src)
{
	char	*dest_ptr;

	dest_ptr = dest;
	while (*src)
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (dest_ptr);
}

static char	*edge_cases(int n)
{
	char	*str;

	if (n == 0)
	{
		str = (char *)malloc(2 * sizeof(char));
		if (str == NULL)
			return (NULL);
		ft_strcpy(str, "0");
		return (str);
	}
	if (n == -2147483648)
	{
		str = (char *)malloc(12 * sizeof(char));
		if (str == NULL)
			return (NULL);
		ft_strcpy(str, "-2147483648");
		return (str);
	}
	return (NULL);
}

static size_t	calc_num_len(int n)
{
	size_t	num_len;

	num_len = 0;
	if (n <= 0)
		num_len++;
	while (n != 0)
	{
		n = n / 10;
		num_len++;
	}
	return (num_len);
}

char	*ft_itoa(int n)
{
	char	*str;
	size_t	num_len;

	if (n == 0 || n == -2147483648)
		return (edge_cases(n));
	num_len = calc_num_len(n);
	str = (char *)malloc((num_len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	str[num_len] = '\0';
	if (n < 0)
	{
		n = -n;
		str[0] = '-';
	}
	while (n > 0)
	{
		str[num_len - 1] = n % 10 + '0';
		n = n / 10;
		num_len--;
	}
	return (str);
}

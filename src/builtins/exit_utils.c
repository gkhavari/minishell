/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 20:10:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 20:18:38 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	parse_exit_value(char *str, long long *value)
{
	char	*end;

	if (!str || !*str)
		return (0);
	errno = 0;
	*value = strtoll(str, &end, 10);
	if (errno == ERANGE || *end != '\0')
		return (0);
	return (1);
}

int	exit_mod256_from_ll(long long value)
{
	return ((unsigned char)value);
}

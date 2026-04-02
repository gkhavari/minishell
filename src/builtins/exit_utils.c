/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 14:58:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/29 18:43:42 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	parse_exit_init(char *str, int *i, int *sign, int *valid)
{
	while (str[*i] && ft_isspace((unsigned char)str[*i]))
		(*i)++;
	*sign = 1;
	if (str[*i] == '+' || str[*i] == '-')
		*sign = 1 - (2 * (str[(*i)++] == '-'));
	*valid = ft_isdigit(str[*i]);
}

static int	parse_exit_accumulate(char *str, int *i,
		unsigned long long *acc, unsigned long long limit)
{
	while (ft_isdigit(str[*i]))
	{
		if (*acc > (limit - (unsigned long long)(str[*i] - '0')) / 10)
			return (0);
		*acc = (*acc * 10) + (unsigned long long)(str[*i] - '0');
		(*i)++;
	}
	return (1);
}

static int	parse_exit_finalize(char *str, int i)
{
	while (str[i] && ft_isspace((unsigned char)str[i]))
		i++;
	if (str[i] != '\0')
		return (0);
	return (1);
}

/** Parse exit numeric string into *value; 0 if invalid or out of range. */
int	parse_exit_value(char *str, long long *value)
{
	unsigned long long	acc;
	unsigned long long	limit;
	int					i;
	int					sign;
	int					valid;

	if (!str)
		return (0);
	i = 0;
	parse_exit_init(str, &i, &sign, &valid);
	if (!valid)
		return (0);
	acc = 0;
	limit = (unsigned long long)LLONG_MAX + (sign < 0);
	if (!parse_exit_accumulate(str, &i, &acc, limit))
		return (0);
	if (!parse_exit_finalize(str, i))
		return (0);
	if (sign < 0 && acc == (unsigned long long)LLONG_MAX + 1ULL)
		*value = LLONG_MIN;
	else
		*value = (long long)(acc * (unsigned long long)sign);
	return (1);
}

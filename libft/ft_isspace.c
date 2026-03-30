/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isspace.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 23:53:50 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/30 23:54:32 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/**
 * ft_isspace - Check if a character is a whitespace character
 * @c: The character to check
 *
 * This function checks if the given character is a whitespace character,
 * which includes space, tab, newline, vertical tab, form feed, and carriage 
 * return.
 *
 * Return: 1 if the character is a whitespace character, 0 otherwise
 */
int	ft_isspace(int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
        || c == '\r');
}
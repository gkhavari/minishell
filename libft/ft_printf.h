/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 21:49:26 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <unistd.h>

# define HEXBASE "0123456789abcdef"
# define DECBASE "0123456789"

int	ft_printf(const char *format, ...);
int	ft_dprintf(int fd, const char *format, ...);

int	print_chr_fd(int fd, char c);
int	print_str_fd(int fd, char *s);
int	print_nbr_fd(int fd, int n);
int	print_hex_fd(int fd, unsigned int n, char c);
int	print_unsigned_fd(int fd, unsigned int n);
int	print_pointer_fd(int fd, unsigned long long addr);

#endif

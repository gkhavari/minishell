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
# include <stddef.h>
# include <unistd.h>

# define HEXBASE "0123456789abcdef"
# define DECBASE "0123456789"
/* dispatch: try_* return this when spec is not handled */
# define PF_CONV_UNHANDLED (-1000)

int	ft_printf(const char *format, ...);
int	ft_dprintf(int fd, const char *format, ...);

int	dispatch_printf_conv(int fd, char spec, va_list ap);

int	ft_write_all(int fd, const void *buf, size_t n);

int	print_chr_fd(int fd, char c);
int	print_str_fd(int fd, char *s);
int	print_nbr_fd(int fd, int n);
int	print_hex_fd(int fd, unsigned int n, char c);
int	print_unsigned_fd(int fd, unsigned int n);
int	print_pointer_fd(int fd, unsigned long long addr);

#endif

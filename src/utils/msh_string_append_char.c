/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_string_append_char.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:23:11 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:23:12 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Grow *dst with `ft_realloc` and append c.
 * On failure frees *dst, sets NULL, returns OOM.
 */
int	append_char(t_shell *shell, char **dst, char c)
{
	size_t	len;
	char	*new;

	(void)shell;
	if (!(*dst))
		len = 0;
	else
		len = ft_strlen(*dst);
	new = ft_realloc(*dst, len + 2);
	if (!new)
		return (free(*dst), (*dst = NULL), OOM);
	*dst = new;
	new[len] = c;
	new[len + 1] = '\0';
	return (SUCCESS);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_word_append_expanded.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 12:23:25 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/04/03 12:23:27 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Append expanded text to *word (quoted or heredoc growth).
 * Returns OOM if `ft_realloc` fails; else SUCCESS.
 */
int	exp_q_cat(char **word, const char *exp)
{
	size_t	len_word;
	size_t	len_exp;
	char	*tmp;

	if (*word)
		len_word = ft_strlen(*word);
	else
		len_word = 0;
	if (exp)
		len_exp = ft_strlen(exp);
	else
		len_exp = 0;
	tmp = ft_realloc(*word, len_word + len_exp + 1);
	if (!tmp)
		return (OOM);
	*word = tmp;
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
	return (SUCCESS);
}

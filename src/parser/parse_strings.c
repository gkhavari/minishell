/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_strings.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 15:07:52 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/02 15:07:56 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*handle_single_quote(const char *input, size_t i, size_t j)
{
	size_t	start;
	size_t	len;
	char	*word;

	start = i + 1;
	len = j - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	ft_memcpy(word, input + start, len);
	word[len] = '\0';
	return (word);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 20:01:07 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/06 20:01:09 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_var(const char *s, size_t *i, t_shell *shell)
{
	size_t	start;
	size_t	len;
	char	buf[12];
	char	*name;
	char	*value;

	start = *i + 1;
	if (s[start] == '?')
	{
		snprintf(buf, sizeof(buf), "%d", shell->last_exit);
		(*i) += 2;
		return (ft_strdup(buf));
	}
	if (!(ft_isalpha(s[start]) || s[start] == '_'))
	{
		(*i)++;
		return (ft_strdup("$"));
	}
	len = 0;
	while (isalnum(s[start + len]) || s[start + len] == '_')
		len++;
	name = ft_strndup(s + start, len);
	value = ft_strdup(get_env_value(shell->envp, name));
	free(name);
	(*i) = start + len;
	return (value);
}

void	append_expansion_quoted(char **word, const char *exp)
{
	size_t	len_word;
	size_t	len_exp;

	if (*word)
		len_word = ft_strlen(*word);
	else
		len_word = 0;
	if (exp)
		len_exp = ft_strlen(exp);
	else
		len_exp = 0;
	*word = realloc(*word, len_word + len_exp + 1); //change to allowed function
	if (!*word)
		exit(1);
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
}

void	append_expansion_unquoted(char **word, const char *exp,
		t_token **tokens)
{
	size_t	i;

	i = 0;
	if (exp == NULL)
		return ;
	while (exp[i])
	{
		if (isspace(exp[i]))
		{
			if (*word)
				flush_word(word, tokens);
			while (isspace(exp[i]))
				i++;
		}
		else
		{
			append_char(word, exp[i]);
			i++;
		}
	}
}

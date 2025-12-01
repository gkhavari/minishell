/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:58 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/29 20:22:01 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_token(t_token **head, t_token *new)
{
	t_token	*tmp;

	if (!(*head))
		*head = new;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

static t_token	*new_token(t_tokentype type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(*token));
	if (!token)
		return (NULL); //todo: error handling
	token->type = type;
	token->value = ft_strdup(value);
	token->next = NULL;
	return (token);
}

void	tokenize_input(t_shell *shell)
{
	char		*s;
	size_t		i;
	t_tokentype	type;
	char		*word;

	s = shell->input;
	shell->tokens = NULL;
	word = NULL;
	i = 0;
	while (s[i])
	{
		if (s[i] == '|')
			parse_pipe(s, &i, &type);
		else if (s[i] == '<')
			parse_heredoc_redir_in(s, &i, &type);
		else if (s[i] == '>')
			parse_append_redir_out(s, &i, &type);
		else if (s[i] == ' ')
			i++;
		else if (s[i] == SINGLE_QUOTE || s[i] == DOUBLE_QUOTE)
			word = parse_sting(s, &i, &type);
		else
			word = parse_word(s, &i, &type);
		if (i != 0 && s[i - 1] != ' ')
			add_token(&shell->tokens, new_token(type, word));
		if (word != NULL)
		{
			free(word);
			word = NULL;
		}
	}
}

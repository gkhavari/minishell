/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:09:05 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:09:12 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_word_boundary(char c)
{
	if (c == '\0' || msh_is_ifs_blank((unsigned char)c))
		return (1);
	return (is_op_char(c));
}

static int	is_redir_target(t_shell *shell, char *word)
{
	t_list	*last_n;
	t_token	*last;

	if (word)
		return (0);
	last_n = ft_lstlast(shell->tokens);
	if (!last_n)
		return (0);
	last = last_n->content;
	return (last->type == REDIR_IN || last->type == REDIR_OUT
		|| last->type == APPEND || last->type == HEREDOC
	);
}

static int	push_ambiguous_redir_token(t_shell *shell, size_t start, size_t end)
{
	char	*raw;
	char	*value;
	t_token	*tok;

	raw = ft_strndup(shell->input + start, end - start);
	if (!raw)
		return (MSH_OOM);
	value = ft_strjoin(MSH_AMBIG_REDIR_PREFIX, raw);
	free(raw);
	if (!value)
		return (MSH_OOM);
	tok = new_token(shell, WORD, value);
	if (!tok)
	{
		free(value);
		return (MSH_OOM);
	}
	free(value);
	if (add_token(&shell->tokens, tok) == MSH_OOM)
		return (MSH_OOM);
	return (MSH_LEX_YES);
}

/**
 * Empty $ expansion at word boundary: empty token or ambiguous redirect.
 */
int	handle_empty_unquoted_expansion(t_shell *shell, size_t start,
		size_t end, char **word)
{
	t_token	*tok;

	if (*word || !is_word_boundary(shell->input[end]))
		return (MSH_LEX_NO);
	if (!is_redir_target(shell, *word))
	{
		tok = new_token(shell, WORD, MSH_EMPTY_EXPAND_TOKEN);
		if (!tok)
			return (MSH_OOM);
		if (add_token(&shell->tokens, tok) == MSH_OOM)
			return (MSH_OOM);
		return (MSH_LEX_YES);
	}
	return (push_ambiguous_redir_token(shell, start, end));
}

/**
 * Append exp to *word (quoted context; no whitespace split).
 * Returns MSH_OOM if ft_realloc fails (*word unchanged).
 */
int	append_expansion_quoted(char **word, const char *exp)
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
		return (MSH_OOM);
	*word = tmp;
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
	return (SUCCESS);
}

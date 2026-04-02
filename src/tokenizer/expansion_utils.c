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

static int	is_redir_target(t_shell *shell, char *word)
{
	t_list	*last_n;
	t_token	*last;

	if (word)
		return (FALSE);
	last_n = ft_lstlast(shell->tokens);
	if (!last_n)
		return (FALSE);
	last = last_n->content;
	if (last->type == REDIR_IN || last->type == REDIR_OUT
		|| last->type == APPEND || last->type == HEREDOC)
		return (TRUE);
	return (FALSE);
}

static int	push_ambg_redir_tok(t_shell *shell, size_t start, size_t end)
{
	char	*raw;
	char	*value;
	t_token	*tok;

	raw = ft_strndup(shell->input + start, end - start);
	if (!raw)
		return (OOM);
	value = ft_strjoin(AMBIG_REDIR, raw);
	free(raw);
	if (!value)
		return (OOM);
	tok = new_token(shell, WORD, value);
	free(value);
	if (!tok || add_token(&shell->tokens, tok) == OOM)
		return (OOM);
	return (LEX_YES);
}

/**
 * Empty $ expansion at word boundary: empty token or ambiguous redirect.
 */
int	handle_empty_unquoted_expansion(t_shell *shell, size_t start,
		size_t end, char **word)
{
	t_token	*tok;

	if (*word || (shell->input[end] != '\0'
			&& !msh_is_ifs_blank((unsigned char)shell->input[end])
			&& !is_op_char(shell->input[end])))
		return (LEX_NO);
	if (is_redir_target(shell, *word))
		return (push_ambg_redir_tok(shell, start, end));
	tok = new_token(shell, WORD, EMPTY_EXPAND);
	if (!tok || add_token(&shell->tokens, tok) == OOM)
		return (OOM);
	return (LEX_YES);
}

/**
 * Append exp to *word (quoted context; no whitespace split).
 * Returns OOM if ft_realloc fails (*word unchanged).
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
		return (OOM);
	*word = tmp;
	ft_memcpy(*word + len_word, exp, len_exp);
	(*word)[len_word + len_exp] = '\0';
	return (SUCCESS);
}

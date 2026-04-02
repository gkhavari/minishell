/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/07 17:01:01 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/07 17:01:03 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * End of line: error if unclosed quote; else add_history on interactive input.
 */
void	handle_end_of_string(t_shell *shell, t_state *state, char **word)
{
	if (*state == ST_SQUOTE || *state == ST_DQUOTE)
	{
		msh_strptr_free(word);
		ft_dprintf(STDERR_FILENO,
			"minishell: syntax error: unclosed quote\n");
		shell->last_exit = XSYN;
	}
	else
	{
		if (isatty(STDIN_FILENO) == 1 && shell->input && shell->input[0])
			add_history(shell->input);
	}
}

/**
 * ST_NORMAL: \\X — skip backslash, append X literally.
 */
int	handle_backslash(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (shell->input[*i] != '\\' || *state != ST_NORMAL)
		return (TOK_N);
	if (!shell->input[*i + 1])
	{
		(*i)++;
		return (TOK_Y);
	}
	if (!*word)
	{
		*word = ft_strdup("");
		if (!*word)
			return (OOM);
	}
	if (append_char(shell, word, shell->input[*i + 1]) == OOM)
		return (OOM);
	*i += 2;
	return (TOK_Y);
}

/**
 * Toggle ST_NORMAL <-> ST_SQUOTE/ST_DQUOTE on quote chars.
 */
int	process_quote(t_shell *shell, char c, t_state *state)
{
	if (*state == ST_NORMAL && c == '\'')
	{
		*state = ST_SQUOTE;
		shell->word_quoted = 1;
		return (TOK_Y);
	}
	if (*state == ST_SQUOTE && c == '\'')
	{
		*state = ST_NORMAL;
		return (TOK_Y);
	}
	if (*state == ST_NORMAL && c == '"')
	{
		*state = ST_DQUOTE;
		shell->word_quoted = 1;
		return (TOK_Y);
	}
	if (*state == ST_DQUOTE && c == '"')
	{
		*state = ST_NORMAL;
		return (TOK_Y);
	}
	return (TOK_N);
}

/**
 * If op char: flush word, append operator token, advance i.
 */
int	handle_operator(t_shell *shell, size_t *i, char **word)
{
	int	n;

	if (!is_op_char(shell->input[*i]))
		return (TOK_N);
	if (flush_word(shell, word, &shell->tokens) == OOM)
		return (OOM);
	if (shell->input[*i] == '<' && shell->input[*i + 1] == '<')
		shell->heredoc_mode = 1;
	n = read_operator(shell, &shell->input[*i], &shell->tokens);
	if (n == OOM)
		return (OOM);
	*i += (size_t)n;
	return (TOK_Y);
}

/**
 * Space/tab: flush word and advance i.
 */
int	handle_whitespace(t_shell *shell, size_t *i, char **word)
{
	if (msh_is_blank((unsigned char)shell->input[*i], 0))
	{
		if (flush_word(shell, word, &shell->tokens) == OOM)
			return (OOM);
		(*i)++;
		return (TOK_Y);
	}
	return (TOK_N);
}

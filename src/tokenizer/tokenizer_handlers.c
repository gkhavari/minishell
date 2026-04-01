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
		free(*word);
		*word = NULL;
		ft_dprintf(STDERR_FILENO,
			"minishell: syntax error: unclosed quote\n");
		shell->last_exit = EXIT_SYNTAX_ERROR;
	}
	else
	{
		if (isatty(STDIN_FILENO) == 1 && shell->input && shell->input[0])
			add_history(shell->input);
	}
}

/**
 * ST_NORMAL: \\X — skip backslash, append X literally. Returns 1 if handled.
 */
int	handle_backslash(t_shell *shell, size_t *i, char **word, t_state *state)
{
	if (shell->input[*i] != '\\' || *state != ST_NORMAL)
		return (0);
	if (!shell->input[*i + 1])
	{
		(*i)++;
		return (1);
	}
	if (!*word)
	{
		*word = ft_strdup("");
		if (!*word)
			return (MSH_OOM);
	}
	if (append_char(shell, word, shell->input[*i + 1]) == MSH_OOM)
		return (MSH_OOM);
	*i += 2;
	return (1);
}

/**
 * Toggle ST_NORMAL <-> ST_SQUOTE/ST_DQUOTE on quote chars.
 * Returns 1 if consumed.
 */
int	process_quote(t_shell *shell, char c, t_state *state)
{
	if (*state == ST_NORMAL && c == '\'')
	{
		*state = ST_SQUOTE;
		mark_word_quoted(shell);
		return (1);
	}
	if (*state == ST_SQUOTE && c == '\'')
	{
		*state = ST_NORMAL;
		return (1);
	}
	if (*state == ST_NORMAL && c == '"')
	{
		*state = ST_DQUOTE;
		mark_word_quoted(shell);
		return (1);
	}
	if (*state == ST_DQUOTE && c == '"')
	{
		*state = ST_NORMAL;
		return (1);
	}
	return (0);
}

/**
 * If op char: flush word, append operator token, advance i.
 * Returns 1 if handled.
 */
int	handle_operator(t_shell *shell, size_t *i, char **word)
{
	int	n;

	if (!is_op_char(shell->input[*i]))
		return (0);
	if (flush_word(shell, word, &shell->tokens) == MSH_OOM)
		return (MSH_OOM);
	if (shell->input[*i] == '<' && shell->input[*i + 1] == '<')
		set_heredoc_mode(shell, 1);
	n = read_operator(shell, &shell->input[*i], &shell->tokens);
	if (n == MSH_OOM)
		return (MSH_OOM);
	*i += (size_t)n;
	return (1);
}

/**
 * Space/tab: flush word and advance i. Returns 1 if handled.
 */
int	handle_whitespace(t_shell *shell, size_t *i, char **word)
{
	if (shell->input[*i] == ' ' || shell->input[*i] == '\t')
	{
		if (flush_word(shell, word, &shell->tokens) == MSH_OOM)
			return (MSH_OOM);
		(*i)++;
		return (1);
	}
	return (0);
}

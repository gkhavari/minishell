/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   continuation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 00:40:08 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/06 00:40:10 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
  DESCRIPTION:
 * Builds a prompt of the form: "<quote_char>quote>".
 * Calls readline() to get user input.
 * Ensures the returned string always ends with a newline ('\n').
 * Allocates and returns a new string containing the user input + '\n'.
 *
  PARAMETERS:
 * quote_char: The opening quote character ('\'' or '"') used to build the
 	prompt.
 *
  RETURN VALUE:
 * Pointer to the newly allocated continuation line including a trailing newline.
 * NULL if readline returns NULL.
 *
  NOTES:
 * Caller is responsible for freeing the returned string.
 **/
static char	*read_continuation_line(t_shell *shell, char quote_char)
{
	char	*line;
	char	*prompt;
	char	tmp[2];
	size_t	len;
	char	*with_newline;

	tmp[0] = quote_char;
	tmp[1] = '\0';
	prompt = ft_strjoin(tmp, "quote>");
	line = readline(prompt);
	if (!line)
		return (NULL);
	len = ft_strlen(line);
	with_newline = msh_calloc(shell, len + 2, sizeof(char));
	ft_memcpy(with_newline, line, len);
	with_newline[len] = '\n';
	with_newline[len + 1] = '\0';
	free(line);
	free(prompt);
	return (with_newline);
}

/**
 DESCRIPTION:
 * Determines which quote character to use for continuation prompts.
 *
  PARAMETERS:
 * state: The current tokenizer state (ST_SQUOTE or ST_DQUOTE).
 *
  RETURN VALUE:
 * SINGLE_QUOTE ('\'') if state == ST_SQUOTE.
 * DOUBLE_QUOTE ('"') otherwise.
 **/
static char	get_quote_char(t_state state)
{
	if (state == ST_SQUOTE)
		return (SINGLE_QUOTE);
	return (DOUBLE_QUOTE);
}

/**
 DESCRIPTION:
 * If the string already ends with '\n', nothing is done.
 * Otherwise:
 ** A new buffer is allocated,
 ** '\n' is appended,
 ** The original string is freed,
 ** *s and *old_len are updated accordingly.
 *
 PARAMETERS:
 * s: Pointer to the input string pointer (char **).
 * old_len: Pointer to the length of the original string.
 *
  RETURN VALUE:
 * None.
 *
  NOTES:
 * This is used before appending continuation lines to maintain
 	correct structure.
 **/
static void	ensure_trailing_newline(t_shell *shell, char **s, size_t *old_len)
{
	char	*tmp;

	if (*old_len > 0 && (*s)[*old_len - 1] == '\n')
		return ;
	tmp = msh_calloc(shell, *old_len + 2, sizeof(char));
	ft_memcpy(tmp, *s, *old_len);
	tmp[*old_len] = '\n';
	tmp[*old_len + 1] = '\0';
	free(*s);
	*s = tmp;
	(*old_len)++;
}

/**
  DESCRIPTION:
 * Appends a continuation string to the existing input buffer.
 *
 PARAMETERS:
 * s: Pointer to the existing input buffer pointer.
 * cont: The continuation line to append.
 * old_len: Length of the existing string *s.
 * cont_len: Length of the continuation string cont.
 *
  RETURN VALUE:
 * None.
 *
 * NOTES:
 * Allocates a new buffer (old_len + cont_len + 1).
 * Frees the old buffer and replaces *s.
 **/
static void	append_to_input(t_shell *shell, char **s, char *cont)
{
	char	*new_input;
	size_t	old_len;
	size_t	cont_len;

	old_len = ft_strlen(*s);
	cont_len = ft_strlen(cont);
	new_input = msh_calloc(shell, old_len + cont_len + 1, sizeof(char));
	ft_memcpy(new_input, *s, old_len);
	ft_memcpy(new_input + old_len, cont, cont_len);
	new_input[old_len + cont_len] = '\0';
	free(*s);
	*s = new_input;
}

/**
  DESCRIPTION:
 * Main continuation handler called inside the tokenizer.
 *
  BEHAVIOR:
 * Determines the correct quote character for the continuation prompt.
 * Reads a continuation line from the user.
 * Ensures the existing input ends with a newline.
 * Appends the continuation line to the existing input.
 *
  PARAMETERS:
 * s: Pointer to the full input buffer pointer (char **).
 * state: Current tokenizer state to determine quote type.
 *
  RETURN VALUE:
 * 1 on success.
 * 0 when readline returns NULL.
 * The resulting full input (including continuations) is stored in *s.
 **/
int	append_continuation(t_shell *shell, char **s, t_state state)
{
	char	quote_char;
	char	*cont;
	size_t	old_len;

	quote_char = get_quote_char(state);
	cont = read_continuation_line(shell, quote_char);
	if (!cont)
		return (0);
	old_len = ft_strlen(*s);
	ensure_trailing_newline(shell, s, &old_len);
	append_to_input(shell, s, cont);
	free(cont);
	return (1);
}

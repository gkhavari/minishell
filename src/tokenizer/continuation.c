/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   continuation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 00:40:08 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:22:35 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Read a line from `stdin` when not running interactively.

 BEHAVIOR:
* Uses `fgets` into a fixed buffer and returns a freshly allocated copy
* (without altering the buffer). Returns NULL on EOF or error.

 PARAMETERS:
* t_shell *shell: Shell runtime used for allocation.

 RETURN:
* Allocated string on success or NULL on EOF/error.
*/
static char	*read_stdin_line(t_shell *shell)
{
	char	buf[4096];
	char	*result;
	size_t	len;

	if (!fgets(buf, sizeof(buf), stdin))
		return (NULL);
	len = ft_strlen(buf);
	result = msh_calloc(shell, len + 1, sizeof(char));
	ft_memcpy(result, buf, len);
	return (result);
}

/**
 DESCRIPTION:
* Read a continuation line from the user when a quoted string is unterminated.

 BEHAVIOR:
* If `stdin` is not a TTY reads a line from stdin. Otherwise prompts with
* a quote prompt and uses `readline`. Returns a newly allocated string
* which always ends with a trailing newline.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation and readline use.
* char quote_char: The quote character that opened the continuation.

 RETURN:
* Allocated string with trailing newline, or NULL if user cancelled/EOF.
*/
static char	*read_continuation_line(t_shell *shell, char quote_char)
{
	char	*line;
	char	*prompt;
	char	tmp[2];
	size_t	len;
	char	*with_newline;

	if (!isatty(STDIN_FILENO))
		return (read_stdin_line(shell));
	tmp[0] = quote_char;
	tmp[1] = '\0';
	prompt = ft_strjoin(tmp, "quote>");
	line = readline(prompt);
	if (!line)
		return (free(prompt), NULL);
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
* Ensure the given string ends with a newline, reallocating when needed.

 BEHAVIOR:
* If `*s` does not end with '\n' allocates a new buffer with an extra
* byte, appends '\n' and replaces `*s`. Updates `*old_len` accordingly.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation.
* char **s: Pointer to the string to ensure trailing newline on.
* size_t *old_len: Pointer to current length of `*s`, updated if changed.
*/
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
* Append the continuation fragment `cont` to the current input string `*s`.

 BEHAVIOR:
* Allocates a new buffer large enough to hold `*s` + `cont`, copies both
* parts, NUL-terminates, frees the old `*s` and assigns the new buffer.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation.
* char **s: Pointer to the current input buffer to be extended.
* char *cont: Continuation string to append (may contain a trailing '\n').
*/
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

int	append_continuation(t_shell *shell, char **s, t_state state)
{
	char	quote_char;
	char	*cont;
	size_t	old_len;

	if (state == ST_SQUOTE)
		quote_char = SINGLE_QUOTE;
	else
		quote_char = DOUBLE_QUOTE;
	cont = read_continuation_line(shell, quote_char);
	if (!cont)
		return (0);
	old_len = ft_strlen(*s);
	ensure_trailing_newline(shell, s, &old_len);
	append_to_input(shell, s, cont);
	free(cont);
	return (1);
}

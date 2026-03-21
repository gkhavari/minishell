/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_ops.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 23:13:19 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:18:36 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Return whether `c` may start an operator token (`|`, `<`, `>`).

 BEHAVIOR:
* Performs a fast character check. Used defensively to avoid invoking
* operator parsing on ordinary characters, preventing misclassification
* of input and avoiding unnecessary allocation work.

 PARAMETERS:
* char c: The character to inspect.

 RETURN:
* `1` if `c` is an operator character, `0` otherwise.
*/
int	is_op_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

/**
 DESCRIPTION:
* Parse an operator token at `s` and append it to `*list` when found.

 BEHAVIOR:
* Recognizes single- and double-character operators (`|`, `<<`, `>>`, `<`,
* `>|`, `>`). Creates and appends the corresponding token. Returns the
* operator width (1 or 2) so the caller can advance the input index.
* Defensively avoids creating tokens for non-operator sequences.

 PARAMETERS:
* t_shell *shell: Shell runtime used for allocations.
* const char *s: Pointer to the input at the operator start.
* t_token **list: Pointer to token list head where the new token will be
  appended.

 RETURN:
* `1` or `2` indicating the number of characters consumed, or `0` when no
  operator was recognized.
*/
size_t	read_operator(t_shell *shell, const char *s, t_token **list)
{
	if (s[0] == '|')
		return (add_token(list, new_token(shell, PIPE, "|")), 1);
	if (s[0] == '<' && s[1] == '<')
		return (add_token(list, new_token(shell, HEREDOC, "<<")), 2);
	if (s[0] == '>' && s[1] == '>')
		return (add_token(list, new_token(shell, APPEND, ">>")), 2);
	if (s[0] == '<')
		return (add_token(list, new_token(shell, REDIR_IN, "<")), 1);
	if (s[0] == '>' && s[1] == '|')
		return (add_token(list, new_token(shell, REDIR_OUT, ">")), 2);
	if (s[0] == '>')
		return (add_token(list, new_token(shell, REDIR_OUT, ">")), 1);
	return (0);
}

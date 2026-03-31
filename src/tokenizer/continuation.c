/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   continuation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 00:40:08 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/28 03:18:02 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_stdin_line(t_shell *shell)
{
	char	buf[4096];
	char	*result;
	size_t	len;

	if (!fgets(buf, sizeof(buf), stdin))
		clean_exit(shell, EXIT_FAILURE);
	len = ft_strlen(buf);
	result = msh_calloc(shell, len + 1, sizeof(char));
	ft_memcpy(result, buf, len);
	return (result);
}

static char	*read_continuation_line(t_shell *shell, char quote_char)
{
	char	*line;
	char	*prompt;
	char	tmp[2];
	size_t	len;
	char	*with_newline;

	if (isatty(STDIN_FILENO) != 1)
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

static void	ensure_trailing_newline(t_shell *shell,
		char **s, size_t *old_len)
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

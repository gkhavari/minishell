/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 20:01:07 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/29 14:27:40 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Handles expansion of special shell variables immediately following $.
* Currently supports $? (last command exit status) and $ followed
by a non-alphanumeric character.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure, which stores last_exit for $?.
* size_t *i: Pointer to the current index in the input string. This index is
updated to skip the expanded variable.

BEHAVIOR:
* If the character after $ is ?
** returns a string containing the value of shell->last_exit.
** Advances *i by 2 to skip $?.
* If the character after $ is not a letter or _
** treats $ literally and returns "$".
** Advances *i by 1.
* Otherwise, returns NULL to indicate this is not a special variable.

RETURN:
* `char *` — A newly allocated string representing the expanded variable
	(caller must free).
* `NULL` — No special variable matched.
**/
static char	*expand_special_var(t_shell *shell, size_t *i)
{
	char	c;

	c = shell->input[*i + 1];
	if (c == '?')
	{
		*i += 2;
		return (ft_itoa(shell->last_exit));
	}
	if (c == '"' || c == '\'' || ft_isdigit(c))
	{
		if (c == '"' || c == '\'')
			*i += 1;
		else
			*i += 2;
		return (ft_strdup(""));
	}
	if (!(ft_isalpha(c) || c == '_'))
	{
		(*i)++;
		return (ft_strdup("$"));
	}
	return (NULL);
}

/**
 DESCRIPTION:
* Expands normal environment variables of the form $VAR_NAME.
* Only letters, digits, and _ are allowed in variable names.

 PARAMETERS:
* t_shell *shell: Pointer to the shell structure containing envp
(environment
variables).
* size_t *i: Pointer to the current index in the input string. Updated to
skip the variable name after expansion.

 BEHAVIOR:
* Reads the variable name starting after $.
* Extracts the name consisting of letters, digits, and underscores.
* Retrieves the variable value from shell->envp (or NULL if it's not there)
* Returns a dynamically allocated string containing the value
(NULL if undefined).
* Frees temporary memory used for the variable name.

 RETURN:
 * `char *` — A newly allocated string representing the variable’s value
 * (caller must free). If undefined, returns an empty string allocation.

 **/
static char	*expand_normal_var(t_shell *shell, size_t *i)
{
	size_t	start;
	size_t	len;
	char	*name;
	char	*value;
	char	c;

	start = *i + 1;
	len = 0;
	while (1)
	{
		c = shell->input[start + len];
		if (!(ft_isalnum(c) || c == '_'))
			break ;
		len++;
	}
	name = ft_strndup(shell->input + start, len);
	value = get_env_value(shell->envp, name);
	free(name);
	*i = start + len;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

/**
 DESCRIPTION:
* Top-level function to expand a variable following $ in the input string.
* Handles special variables first, then normal environment variables.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure.
* size_t *i: Pointer to the current index in the input string.

BEHAVIOR:
* Calls expand_special_var().
** If a special variable is matched, returns the expanded string.
* Otherwise, calls expand_normal_var() to handle normal environment variables.

RETURN:
* `char *` — Newly allocated string containing the variable’s value.
**/
char	*expand_var(t_shell *shell, size_t *i)
{
	char	*res;

	res = expand_special_var(shell, i);
	if (res)
		return (res);
	return (expand_normal_var(shell, i));
}

/**
 DESCRIPTION:
* Handles variable expansion in the main tokenizer loop when a $ is encountered.
* Appends the expansion to the current word buffer and splits words if the
* expansion occurs outside quotes.

PARAMETERS:
* t_shell *shell: Pointer to the shell structure containing input and tokens.
* size_t *i: Pointer to the current index in the input string.
* char **word: Pointer to the current word buffer being built.

BEHAVIOR:
* Checks if the current character is $.
* If not, returns 0.
* Calls expand_var() to get the expanded string.
* Appends the expansion to the word buffer using append_expansion_unquoted().
* This ensures that whitespace in the expansion splits the current word into
* multiple tokens if necessary.
* Frees the temporary expanded string.
* Returns 1 to indicate the character was handled.

RETURN:
* `1` if `$` was found and expansion was handled.
* `0` if no expansion was performed.
**/
int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;

	if (shell->input[*i] != '$')
		return (0);
	expanded = expand_var(shell, i);
	append_expansion_unquoted(shell, word, expanded, &shell->tokens);
	free(expanded);
	return (1);
}

/**
 DESCRIPTION:
* Handles tilde expansion when a ~ is encountered at the start of a word.
* Only expands if ~ is the first character and is followed by a valid separator
* (/, space, tab, or end of string). Otherwise, ~ is treated literally.
* Appends the HOME directory value to the current word buffer.
* Advances the input index appropriately to skip the ~ and any following
* separator.
This function is called in the main tokenizer loop when a ~ is encountered.
If HOME is not set, it appends an empty string (effectively removing the ~).

PARAMETERS:
* t_shell *shell: Pointer to the shell structure containing input and tokens.
* size_t *i: Pointer to the current index in the input string. Updated to skip
the ~ and any following separator.
* char **word: Pointer to the current word buffer being built. The expansion is
appended to this buffer.
BEHAVIOR:
* Checks if the current character is `~` and if it's the start of a word
	(i.e. `*word` is NULL). If not, returns `0`.
* Ensures the character following `~` is a valid separator (`/`, space, tab, 
	operator, or end-of-string). If not, treats `~` literally and returns `0`.
* Retrieves the `HOME` value from `shell->envp` and appends it (or an empty
	string when unset) via `append_expansion_unquoted`.
* Advances `*i` past the `~` and returns `1` on success.

RETURN:
* `1` if `~` was handled (expanded or treated literally), otherwise `0`.
**/
int	handle_tilde_expansion(t_shell *shell, size_t *i, char **word)
{
	char	next;
	char	*home;

	if (shell->input[*i] != '~' || *word)
		return (0);
	next = shell->input[*i + 1];
	if (next && next != '/' && next != ' ' && next != '\t'
		&& !is_op_char(next))
		return (0);
	home = get_env_value(shell->envp, "HOME");
	if (!home)
		home = "";
	append_expansion_unquoted(shell, word, home, &shell->tokens);
	(*i)++;
	return (1);
}

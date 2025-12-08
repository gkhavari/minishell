/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 20:01:07 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/06 20:01:09 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* Handles expansion of special shell variables immediately following $.
* Currently supports $? (last command exit status) 
	and $ followed by a non-alphanumeric character.

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

RETURN VALUE:
* char *: A newly allocated string representing the expanded variable 
	(caller must free).
* NULL: No special variable matched.
**/
static char	*expand_special_var(t_shell *shell, size_t *i)
{
	size_t	start;
	char	buf[12];

	start = *i + 1;
	if (shell->input[start] == '?')
	{
		snprintf(buf, sizeof(buf), "%d", shell->last_exit);
		*i += 2;
		return (ft_strdup(buf));
	}
	if (!(ft_isalpha(shell->input[start]) || shell->input[start] == '_'))
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
	(environment variables).
* size_t *i: Pointer to the current index in the input string. Updated to
	skip the variable name after expansion.

 BEHAVIOR:
* Reads the variable name starting after $.
* Extracts the name consisting of letters, digits, and underscores.
* Retrieves the variable value from shell->envp (or NULL if it's not there)
* Returns a dynamically allocated string containing the value
	(NULL if undefined).
* Frees temporary memory used for the variable name.

 RETURN VALUE: 
 * A newly allocated string representing the variable’s value (caller must free).
 */
static char	*expand_normal_var(t_shell *shell, size_t *i)
{
	size_t	start;
	size_t	len;
	char	*name;
	char	*value;

	start = *i + 1;
	len = 0;
	while (ft_isalnum(shell->input[start + len])
		|| shell->input[start + len] == '_')
		len++;
	name = ft_strndup(shell->input + start, len);
	value = ft_strdup(get_env_value(shell->envp, name));
	free(name);
	*i = start + len;
	return (value);
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

RETURN VALUE:
* char * — Newly allocated string containing the variable’s value.
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
	expansion occurs outside quotes.

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
	multiple tokens if necessary.
* Frees the temporary expanded string.
* Returns 1 to indicate the character was handled.

RETURN VALUE:
* 1: $ was found and expansion was handled.
* 0: No expansion performed.
**/
int	handle_variable_expansion(t_shell *shell, size_t *i, char **word)
{
	char	*expanded;

	if (shell->input[*i] != '$')
		return (0);
	expanded = expand_var(shell, i);
	append_expansion_unquoted(word, expanded, &shell->tokens);
	free(expanded);
	return (1);
}

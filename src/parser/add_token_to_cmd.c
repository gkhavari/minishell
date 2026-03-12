/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_token_to_cmd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:14 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** handle_heredoc_token - Store heredoc delimiter from the next token
** Frees any previous delimiter (handles multiple heredocs per command).
** The actual heredoc reading happens later in process_heredocs().
*/
static void	handle_heredoc_token(t_command *cmd, t_token *token)
{
	if (cmd->heredoc_delim)
		free(cmd->heredoc_delim);
	cmd->heredoc_delim = ft_strdup(token->next->value);
	cmd->heredoc_quoted = token->next->quoted;
}

/*
** append_out_redir - Append an output redirection to the command's list.
** Builds a linked list so all output redirections are applied in order.
*/
static void	append_out_redir(t_command *cmd, char *file, int append)
{
	t_redir	*r;
	t_redir	*tmp;

	r = malloc(sizeof(t_redir));
	if (!r)
		return ;
	r->file = ft_strdup(file);
	r->append = append;
	r->next = NULL;
	if (!cmd->out_redirs)
		cmd->out_redirs = r;
	else
	{
		tmp = cmd->out_redirs;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = r;
	}
}

/*
** add_token_to_command - Dispatch a token into the command structure
** WORD tokens become command arguments.
** Redirection tokens (< > >> <<) set the appropriate file/delimiter.
** Note: parse_tokens() already skips the filename WORD after redirections.
*/
void	add_token_to_command(t_command *cmd, t_token *token)
{
	if (token->type == WORD)
		add_word_to_cmd(cmd, token->value);
	else if (token->type == REDIR_IN)
	{
		free(cmd->input_file);
		cmd->input_file = ft_strdup(token->next->value);
	}
	else if (token->type == REDIR_OUT)
		append_out_redir(cmd, token->next->value, 0);
	else if (token->type == APPEND)
		append_out_redir(cmd, token->next->value, 1);
	else if (token->type == HEREDOC)
		handle_heredoc_token(cmd, token);
}

/**
 DESCRIPTION:
 * Appends a new argument to the command’s argument list.
 * This list is a simple linked list of t_arg nodes, each storing one
 	argument string.

PARAMETERS: 
* cmd: The command receiving the new argument.
* word: The raw argument string to duplicate and store.

BEHAVIOR:
* Allocates a new t_arg node.
* Duplicates word into new->value.
* If cmd->args is empty, the new node becomes the head.
* Otherwise, it is appended to the end of the argument list.
**/
static void	add_word_to_cmd(t_shell *shell, t_command *cmd, char *word)
{
	t_arg	*new;
	t_arg	*tmp;

	new = msh_calloc(shell, 1, sizeof(t_arg));
	new->value = ft_strdup(word);
	new->next = NULL;
	if (!cmd->args)
		cmd->args = new;
	else
	{
		tmp = cmd->args;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

/**
 DESCRIPTION:
* Duplicates a string safely and assigns it to a destination pointer.
* Returns SUCCESS if duplication succeeds, FAILURE if memory allocation fails
* or if the input value is NULL.

 PARAMETERS:
* dest: Address of the pointer to assign the duplicated string.
* value: The string to duplicate.

 RETURN:
* SUCCESS if duplication succeeds.
* FAILURE if strdup fails or value is NULL.
**/
static int	set_redir_file(char **dest, const char *value)
{
	char	*tmp;

	if (!value)
		return (FAILURE);
	tmp = ft_strdup(value);
	if (!tmp)
		return (FAILURE);
	*dest = tmp;
	return (SUCCESS);
}

/**
 DESCRIPTION:
* Processes a token and updates the corresponding fields of a
	t_command structure.
* Depending on the token type, the function either:
** Adds a word to the command’s argument list
** Sets input or output redirection
** Marks output redirection as append (>>)
** Processes a heredoc (<<)
* This function assumes that redirection and heredoc tokens always have a
	valid token->next pointing to a WORD token.
* This is guaranteed by syntax_check() before parsing.

 PARAMETERS:
* shell: Pointer to the shell context, required for heredoc processing.
* cmd: Pointer to the command currently being built.
* token: The token to interpret and apply to the command.

 BEHAVIOR BY TOKEN TYPE:
* WORD: Adds the token’s value to the command arguments via add_word_to_cmd().
* REDIR_IN (<): Sets cmd->input_file to the filename following the token.
* REDIR_OUT (>): Sets cmd->output_file and clears the append flag.
* APPEND (>>): Sets cmd->output_file and enables the append flag.
* HEREDOC (<<): Calls process_heredoc()
	using the following token as the delimiter.

 RETURN:
* 1 for WORD tokens
* 2 for redirection or heredoc tokens
* FAILURE if memory allocation or heredoc processing fails.
**/
int	add_token_to_command(t_shell *shell, t_command *cmd, t_token *token)
{
	if (token->type == WORD)
	{
		add_word_to_cmd(shell, cmd, token->value);
		return (1);
	}
	else if (token->type == REDIR_IN && token->next)
	{
		if (set_redir_file(&cmd->input_file, token->next->value) == FAILURE)
			return (FAILURE);
		return (2);
	}
	else if ((token->type == REDIR_OUT || token->type == APPEND) && token->next)
	{
		if (set_redir_file(&cmd->output_file, token->next->value) == FAILURE)
			return (FAILURE);
		cmd->append = (token->type == APPEND);
		return (2);
	}
	else if (token->type == HEREDOC && token->next)
	{
		if (process_heredoc(shell, cmd, token->next->value) == FAILURE)
			return (FAILURE);
		return (2);
	}
	return (1);
}

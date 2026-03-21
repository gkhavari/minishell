/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/21 22:19:41 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/**
 DESCRIPTION:
* Allocate and initialize a new `t_command` node.

 BEHAVIOR:
* Uses `msh_calloc` to zero the structure and sets the `heredoc_fd`
* field to -1 to indicate no open heredoc. Caller owns the result.

 PARAMETERS:
* t_shell *shell: Shell runtime used for allocation and error handling.

 RETURN:
* Pointer to the newly allocated `t_command`, or NULL on allocation failure.
*/
static t_command	*new_command(t_shell *shell)
{
	t_command	*cmd;

	cmd = msh_calloc(shell, 1, sizeof(t_command));
	if (cmd)
		cmd->heredoc_fd = -1;
	return (cmd);
}

/**
 DESCRIPTION:
* Consume tokens for a single command, dispatching them into `cmd`.

 BEHAVIOR:
* Calls `add_token_to_command` to process the current token. Advances
* the token pointer by the number of consumed tokens and returns the
* next unprocessed token. On error frees the partially-built command
* and returns NULL.

 PARAMETERS:
* t_shell *shell: Shell runtime for allocation and helpers.
* t_command *cmd: Command node being populated.
* t_token *token: First token to process.

 RETURN:
* Next unprocessed `t_token` pointer, or NULL on error.
*/
static t_token	*consume_command_tokens(t_shell *shell, t_command *cmd,
	t_token *token)
{
	int	consumed;

	consumed = add_token_to_command(shell, cmd, token);
	if (consumed == FAILURE)
	{
		free_commands(cmd);
		return (NULL);
	}
	while (consumed > 0 && token)
	{
		token = token->next;
		consumed--;
	}
	return (token);
}

/**
 DESCRIPTION:
* Build a linked list of `t_command` from the token stream.

 BEHAVIOR:
* Allocates the head command and iterates tokens, splitting commands
* on `PIPE` tokens and delegating token consumption to
* `consume_command_tokens`.

 PARAMETERS:
* t_shell *shell: Shell runtime used for allocations.
* t_token *token: Head of the token list to parse.

 RETURN:
* Head of the constructed `t_command` list, or NULL on failure.
*/
static t_command	*parse_tokens(t_shell *shell, t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command(shell);
	cmd = head;
	while (token)
	{
		if (token->type == PIPE)
		{
			cmd->next = new_command(shell);
			cmd = cmd->next;
			token = token->next;
		}
		else
			token = consume_command_tokens(shell, cmd, token);
	}
	return (head);
}

/**
 DESCRIPTION:
* Top-level parsing entry point that converts tokens into commands.

 BEHAVIOR:
* Validates token syntax with `syntax_check`. On success builds the
* command list via `parse_tokens` and performs finalization steps.
* On syntax error sets `shell->last_exit` and frees token data.

 PARAMETERS:
* t_shell *shell: Shell runtime containing `tokens` and where
*                 `commands` will be stored.

 RETURN:
* None.
*/
void	parse_input(t_shell *shell)
{
	if (syntax_check(shell->tokens) == SYNTAX_ERR)
	{
		shell->last_exit = EXIT_SYNTAX_ERROR;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	shell->commands = parse_tokens(shell, shell->tokens);
	finalize_all_commands(shell, shell->commands);
}

/**
 DESCRIPTION:
* Check whether a token type represents a redirection operator.

 PARAMETERS:
* t_tokentype type: Token type to inspect.

 RETURN:
* `1` if `type` is a redirection (<, >, >>, <<), otherwise `0`.
*/
int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT || type == APPEND
		|| type == HEREDOC);
}

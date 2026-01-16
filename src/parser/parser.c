/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 20:21:00 by gkhavari          #+#    #+#             */
/*   Updated: 2026/01/16 16:09:08 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Create a fresh command structure */

t_command	*new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->argv = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = 0;
	cmd->is_builtin = 0;
	cmd->heredoc_delim = NULL;
	cmd->heredoc_fd = -1;
	cmd->next = NULL;
	return (cmd);
}

/* Parse a flat token list into a linked list of commands */

t_command	*parse_tokens(t_token *token)
{
	t_command	*head;
	t_command	*cmd;

	head = new_command();
	cmd = head;
	while (token)
	{
		if (token->type == PIPE)
		{
			cmd->next = new_command();
			cmd = cmd->next;
		}
		else
			add_token_to_command(cmd, token);
		token = token->next;
	}
	return (head);
}

/* Top-level parse entry used by main loop */

void	parse_input(t_shell *shell)
{
	if (syntax_check(shell->tokens))
	{
		shell->last_exit = 2;
		return ;
	}
	shell->commands = parse_tokens(shell->tokens);
	finalize_all_commands(shell->commands);
}

/* ==================================================================== */
/* Additional command helpers and heredoc handling (from stash)         */
/* ==================================================================== */

void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	free(cmd->input_file);
	free(cmd->output_file);
	free(cmd->heredoc_delim);
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
	}
	if (cmd->heredoc_fd != -1)
		close(cmd->heredoc_fd);
	free(cmd);
}

/* Check if token is a redirection */
int	is_redirection(t_tokentype type)
{
	return (type == REDIR_IN || type == REDIR_OUT || type == APPEND || type == HEREDOC);
}

/* Process all heredocs before execution */
int	process_heredocs(t_shell *shell)
{
	t_command	*cmd;

	cmd = shell->commands;
	while (cmd)
	{
		if (cmd->heredoc_delim)
		{
			if (read_heredoc(cmd, shell))
				return (1); /* error or interrupted */
		}
		cmd = cmd->next;
	}
	return (0);
}

/* Read heredoc input until delimiter and write to a pipe */
int	read_heredoc(t_command *cmd, t_shell *shell)
{
	int		pipe_fd[2];
	char	*line;
	int		expand;

	if (pipe(pipe_fd) == -1)
		return (1);
	cmd->heredoc_fd = pipe_fd[0]; /* read end */
	/* Check if delimiter is quoted (no expansion) */
	expand = !is_quoted_delimiter(cmd->heredoc_delim);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			/* EOF, bash warns */
			ft_putstr_fd("minishell: warning: here-document delimited by EOF\n", 2);
			break ;
		}
		if (g_signum == SIGINT)
		{
			free(line);
			close(pipe_fd[1]);
			return (1); /* interrupted */
		}
		if (ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		if (expand)
			line = expand_heredoc_line(line, shell);
		ft_putendl_fd(line, pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	return (0);
}

/* Check if heredoc delimiter is quoted (no expansion) */
int	is_quoted_delimiter(char *delim)
{
	/* Simple check: if starts and ends with quotes */
	if (!delim || !*delim)
		return (0);
	if ((delim[0] == '"' && delim[ft_strlen(delim) - 1] == '"') ||
		(delim[0] == '\'' && delim[ft_strlen(delim) - 1] == '\''))
		return (1);
	return (0);
}

/* Expand variables in heredoc line if not quoted */
char	*expand_heredoc_line(char *line, t_shell *shell)
{
	(void)shell;
	/* For now, simple implementation: just return line */
	return (ft_strdup(line));
}

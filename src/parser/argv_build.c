/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:23 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	cleanup_partial_argv(char **argv, size_t count)
{
	while (count > 0)
	{
		count--;
		free(argv[count]);
		argv[count] = NULL;
	}
	free(argv);
}

/**
 * Build cmd->argv from cmd->args (NULL-terminated). Returns 0 or -1 on OOM.
 */
static int	build_argv_array(t_command *cmd, t_list *arg_node, size_t count)
{
	size_t	i;
	t_arg	*a;

	i = 0;
	while (i < count && arg_node)
	{
		a = arg_node->content;
		cmd->argv[i] = ft_strdup(a->value);
		if (!cmd->argv[i])
		{
			cleanup_partial_argv(cmd->argv, i);
			cmd->argv = NULL;
			return (-1);
		}
		arg_node = arg_node->next;
		i++;
	}
	cmd->argv[count] = NULL;
	return (0);
}

static int	finalize_argv(t_shell *shell, t_command *cmd)
{
	size_t	count;

	(void)shell;
	count = (size_t)ft_lstsize(cmd->args);
	cmd->argv = ft_calloc(count + 1, sizeof(char *));
	if (!cmd->argv)
		return (-1);
	return (build_argv_array(cmd, cmd->args, count));
}

/**
 * For each command: build argv, set is_builtin from get_builtin_type (env rule).
 * Returns 0 or -1 on allocation failure.
 */
int	finalize_all_commands(t_shell *shell, t_list *cmd_list)
{
	t_list		*node;
	t_command	*cmd;

	node = cmd_list;
	while (node)
	{
		cmd = node->content;
		if (finalize_argv(shell, cmd) < 0)
			return (-1);
		cmd->is_builtin = (get_builtin_type(cmd->argv[0]) != NOT_BUILTIN);
		if (cmd->is_builtin && cmd->argv[1]
			&& ft_strcmp(cmd->argv[0], "env") == 0)
			cmd->is_builtin = 0;
		node = node->next;
	}
	return (0);
}

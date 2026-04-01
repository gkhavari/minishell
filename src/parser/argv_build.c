/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argv_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:01:23 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
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
 * Build cmd->argv from cmd->args (NULL-terminated); exit on strdup failure.
 */
static void	build_argv_array(t_shell *shell, t_command *cmd,
		t_arg *tmp, size_t count)
{
	size_t	i;

	i = 0;
	while (i < count)
	{
		cmd->argv[i] = ft_strdup(tmp->value);
		if (!cmd->argv[i])
		{
			cleanup_partial_argv(cmd->argv, i);
			cmd->argv = NULL;
			clean_exit(shell, EXIT_FAILURE);
		}
		tmp = tmp->next;
		i++;
	}
	cmd->argv[count] = NULL;
}

static void	finalize_argv(t_shell *shell, t_command *cmd)
{
	t_arg	*tmp;
	size_t	count;

	(void)shell;
	tmp = cmd->args;
	count = 0;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	cmd->argv = ft_calloc(count + 1, sizeof(char *));
	if (!cmd->argv)
		clean_exit(shell, EXIT_FAILURE);
	build_argv_array(shell, cmd, cmd->args, count);
}

/**
 * For each command: build argv, set is_builtin (with env special-case).
 */
void	finalize_all_commands(t_shell *shell, t_command *cmd)
{
	while (cmd)
	{
		finalize_argv(shell, cmd);
		cmd->is_builtin = is_builtin(cmd->argv[0]);
		if (cmd->is_builtin && cmd->argv[1]
			&& ft_strcmp(cmd->argv[0], "env") == 0)
			cmd->is_builtin = 0;
		cmd = cmd->next;
	}
}

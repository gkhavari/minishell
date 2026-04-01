/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:26 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/30 20:29:27 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Print envp lines; error if extra arg or NULL envp. */
int	builtin_env(char **args, t_shell *shell)
{
	int	i;

	if (args[1])
	{
		ft_dprintf(STDERR_FILENO,
			"env: '%s': No such file or directory\n", args[1]);
		return (EXIT_CMD_NOT_FOUND);
	}
	if (!shell->envp)
		return (FAILURE);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_printf("%s\n", shell->envp[i]);
		i++;
	}
	return (SUCCESS);
}

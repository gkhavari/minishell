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
		ft_putstr_fd("env: '", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd("': No such file or directory", 2);
		return (127);
	}
	if (!shell->envp)
		return (FAILURE);
	i = 0;
	while (shell->envp[i])
	{
		if (ft_strchr(shell->envp[i], '='))
			ft_putendl_fd(shell->envp[i], 1);
		i++;
	}
	return (SUCCESS);
}

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

/*
** builtin_env - Print all environment variables
** @args: unused
** @shell: shell state containing envp
** Return: 0 on success, 1 if envp is NULL
*/
int	builtin_env(char **args, t_shell *shell)
{
	int	i;

	(void)args;
	if (!shell->envp)
		return (1);
	i = 0;
	while (shell->envp[i])
	{
		ft_putendl_fd(shell->envp[i], 1);
		i++;
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/30 20:29:50 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** builtin_pwd - Print current working directory
** @args: unused
** @shell: shell state for fallback cwd
** Return: 0 on success, 1 on failure
*/
int	builtin_pwd(char **args, t_shell *shell)
{
	char	*cwd;

	(void)args;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		if (shell->cwd)
			cwd = ft_strdup(shell->cwd);
		if (!cwd)
			return (1);
	}
	ft_putendl_fd(cwd, 1);
	free(cwd);
	return (0);
}

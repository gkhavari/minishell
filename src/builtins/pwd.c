/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/01 18:15:43 by thanh-ng         ###   ########.fr       */
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
	if (!shell->cwd)
		return (1);
	cwd = ft_strdup(shell->cwd); //this generates a non-freeable string, will be a problem
	if (!cwd)
	{
		perror("minishell: pwd: malloc");
		return (1);
	}
	ft_putendl_fd(cwd, 1);
	free(cwd);
	return (0);
}

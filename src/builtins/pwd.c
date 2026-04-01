/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/01 18:18:42 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/** Print shell->cwd. */
int	builtin_pwd(char **args, t_shell *shell)
{
	(void)args;
	if (!shell->cwd)
		return (FAILURE);
	ft_putendl_fd(shell->cwd, 1);
	return (SUCCESS);
}

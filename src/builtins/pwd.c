/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/30 20:29:49 by thanh-ng          #+#    #+#             */
/*   Updated: 2026/03/21 18:07:41 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 DESCRIPTION:
* `pwd` builtin: print the current working directory.

 BEHAVIOR:
* Writes `shell->cwd` to stdout followed by a newline. Returns an error
* when `shell->cwd` is not available.

 PARAMETERS:
* char **args: Argument vector (unused).
* t_shell *shell: Shell runtime containing `cwd`.

 RETURN:
* `0` on success, non-zero if `cwd` is unavailable.
*/
int	builtin_pwd(char **args, t_shell *shell)
{
	(void)args;
	if (!shell->cwd)
		return (1);
	ft_putendl_fd(shell->cwd, 1);
	return (0);
}

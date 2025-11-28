/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:27 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:28 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `pwd` builtin scaffolding.
 */
#include "minishell.h"

int	builtin_pwd(char **args, t_shell *shell)
{
    char *current_directory;

    (void) args;
    (void) shell;
    current_directory = getcwd(NULL, 0);
    if (!current_directory)
    {
        perror("pwd");
        return (1);
    }
    printf("%s\n", current_directory);
    free(current_directory);
    return (0);
}

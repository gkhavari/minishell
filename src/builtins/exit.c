/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:20 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:21 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `exit` builtin: exit the process with an optional status.
 */
#include "minishell.h"

int builtin_exit(char **args, t_shell *shell)
{
    int status = 0;

    (void) shell;
    if (args && args[1])
        status = atoi(args[1]);
    exit(status);
    return (0); /* unreachable */
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:29:12 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/11/27 20:29:13 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Minimal `echo` builtin: support -n and plain printing.
 */
#include "minishell.h"

int	builtin_echo(char **args, t_shell *shell)
{
    int i = 1;
    int newline = 1;

    (void) shell;
    if (!args)
        return (0);
    if (args[1] && args[1][0] == '-' && args[1][1] == 'n' && args[1][2] == '\0')
    {
        newline = 0;
        i = 2;
    }
    while (args[i])
    {
        if (i > (newline ? 1 : 2))
            putchar(' ');
        fputs(args[i], stdout);
        i++;
    }
    if (newline)
        putchar('\n');
    return (0);
}

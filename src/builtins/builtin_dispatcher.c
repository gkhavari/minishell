/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:30:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/08 16:55:20 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Get builtin type enum
** Returns: enum value for builtin type
*/
t_builtin   get_builtin_type(char *cmd)
{
    if (!cmd)
        return (NOT_BUILTIN);
    if (ft_strcmp(cmd, "echo") == 0)
        return (BUILTIN_ECHO);
    if (ft_strcmp(cmd, "cd") == 0)
        return (BUILTIN_CD);
    if (ft_strcmp(cmd, "pwd") == 0)
        return (BUILTIN_PWD);
    if (ft_strcmp(cmd, "export") == 0)
        return (BUILTIN_EXPORT);
    if (ft_strcmp(cmd, "unset") == 0)
        return (BUILTIN_UNSET);
    if (ft_strcmp(cmd, "env") == 0)
        return (BUILTIN_ENV);
    if (ft_strcmp(cmd, "exit") == 0)
        return (BUILTIN_EXIT);
    return (NOT_BUILTIN);
}

/*
** Check if command is a builtin
** Returns: 1 if builtin, 0 otherwise
*/
int	is_builtin(char *cmd)
{
    return (get_builtin_type(cmd) != NOT_BUILTIN);
}

/*
** Run builtin command and return exit status
** argv[0] is the command name
** Returns: exit status of builtin
*/
int	run_builtin(char **argv, t_shell *shell)
{
    t_builtin type;

    if (!argv || !argv[0])
        return (1);
    type = get_builtin_type(argv[0]);
    if (type == BUILTIN_ECHO)
        return (builtin_echo(argv, shell));
    if (type == BUILTIN_CD)
        return (builtin_cd(argv, shell));
    if (type == BUILTIN_PWD)
        return (builtin_pwd(argv, shell));
    if (type == BUILTIN_EXPORT)
        return (builtin_export(argv, shell));
    if (type == BUILTIN_UNSET)
        return (builtin_unset(argv, shell));
    if (type == BUILTIN_ENV)
        return (builtin_env(argv, shell));
    if (type == BUILTIN_EXIT)
        return (builtin_exit(argv, shell));
    return (1);
}

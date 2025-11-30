/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 21:18:11 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/30 19:41:28 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "includes.h"
# include "defines.h"
# include "structs.h"
# include "prototypes.h"

/* builtins */
int	builtin_cd(char **args, t_shell *shell);
int	builtin_pwd(char **args, t_shell *shell);
int	builtin_env(char **args, t_shell *shell);
int	builtin_echo(char **args, t_shell *shell);
int	builtin_export(char **args, t_shell *shell);
int	builtin_unset(char **args, t_shell *shell);
int	builtin_exit(char **args, t_shell *shell);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototypes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:01 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/30 20:21:43 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTOTYPES_H
# define PROTOTYPES_H

char	*build_prompt(t_shell *shell);
char	*get_env(char **envp, const char *key);
void	init_shell(t_shell *shell, char **envp);
char	*ft_strcat(char *dest, const char *src);
char	*ft_realloc(char *ptr, const size_t new_size);
void	tokenize_input(t_shell *shell);
void	parse_pipe(char *s, size_t *i, t_tokentype *type);
void	parse_heredoc_redir_in(char *s, size_t *i, t_tokentype *type);
void	parse_append_redir_out(char *s, size_t *i, t_tokentype *type);
char	*parse_word(char *s, size_t *i, t_tokentype *type);
void	free_all(t_shell *shell);
char	**ft_arrdup(char **envp);

/* builtins */
int	builtin_cd(char **args, t_shell *shell);
int	builtin_pwd(char **args, t_shell *shell);
int	builtin_env(char **args, t_shell *shell);
int	builtin_echo(char **args, t_shell *shell);
int	builtin_export(char **args, t_shell *shell);
int	builtin_unset(char **args, t_shell *shell);
int	builtin_exit(char **args, t_shell *shell);

/* export_utils.c */
int		is_valid_export_name(char *name);
int		find_export_key_index(t_shell *shell, char *key, int key_len);
int		append_export_env(t_shell *shell, char *entry);

#endif

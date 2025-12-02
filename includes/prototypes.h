/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototypes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:01 by gkhavari          #+#    #+#             */
/*   Updated: 2025/11/29 14:11:06 by gkhavari         ###   ########.fr       */
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
void	parse_pipe(const char *s, size_t *i, t_tokentype *type);
void	parse_heredoc_redir_in(const char *s, size_t *i, t_tokentype *type);
void	parse_append_redir_out(const char *s, size_t *i, t_tokentype *type);
char	*parse_word(const char *s, size_t *i, t_tokentype *type);
void	free_all(t_shell *shell);
char	**ft_arrdup(char **envp);
char	*parse_sting(const char *s, size_t *i, t_tokentype *type);
char	*handle_single_quote(const char *input, size_t i, size_t j);


#endif

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
char	*get_env_value(char **envp, const char *key);
void	init_shell(t_shell *shell, char **envp);
char	*ft_strcat(char *dest, const char *src);
char	*ft_realloc(char *ptr, const size_t new_size);
void	tokenize_input(t_shell *shell);
int		handle_end_of_string(t_shell *shell, t_state *state);
int		process_quote(char c, t_state *state);
int		handle_single_quote(t_shell *shell, size_t *i, char **word,
			t_state *state);
int		handle_double_quote(t_shell *shell, size_t *i, char **word,
			t_state *state);
int		handle_variable_expansion(t_shell *shell, size_t *i, char **word);
int		handle_operator(t_shell *shell, size_t *i, char **word);
int		is_op_char(char c);
size_t	read_operator(const char *s, t_token **list);
int		handle_whitespace(t_shell *shell, size_t *i, char **word);
void	process_normal_char(char c, size_t *i, char **word);
void	flush_word(char **word, t_token **token);
void	add_token(t_token **head, t_token *new);
t_token	*new_token(t_tokentype type, char *value);
void	append_char(char **dst, char c);
int		append_continuation(char **s, t_state state);
char	*expand_var(t_shell *shell, size_t *i);
void	append_expansion_quoted(char **word, const char *exp);
void	append_expansion_unquoted(char **word, const char *exp,
			t_token **tokens);
void	free_all(t_shell *shell);
void	free_tokens(t_token *token);
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

/* signal_handler.c */
int		set_signals_default(void);
int		set_signals_ignore(void);
int		set_signals_interactive(void);
int		handle_child_exit(int *last_exit_status, pid_t pid);
int		check_signal_received(t_shell *shell);

/* builtin_dispatcher.c */
int		is_builtin(char *cmd);
int		run_builtin(char **argv, t_shell *shell);

/* simple_split.c (for testing) */
char	**simple_split_input(char *input);
void	free_simple_argv(char **argv);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototypes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:01 by gkhavari          #+#    #+#             */
/*   Updated: 2026/01/16 16:32:33 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTOTYPES_H
# define PROTOTYPES_H

/* init.c */
char	*build_prompt(t_shell *shell);
void	init_shell(t_shell *shell, char **envp);

/* utils.c */
char	*get_env_value(char **envp, const char *key);
char	*get_env(char **envp, const char *key);
char	*ft_strcat(char *dest, const char *src);
char	*ft_realloc(char *ptr, const size_t new_size);
char	**ft_arrdup(char **envp);

/* tokenizer.c */
void	tokenize_input(t_shell *shell);

/* tokenizer_utils.c */
void	flush_word(char **word, t_token **token);
void	add_token(t_token **head, t_token *new);
t_token	*new_token(t_tokentype type, char *value);
void	append_char(char **dst, char c);

/* tokenizer_handlers.c */
int		handle_end_of_string(t_shell *shell, t_state *state);
int		process_quote(char c, t_state *state);
int		handle_operator(t_shell *shell, size_t *i, char **word);
int		handle_whitespace(t_shell *shell, size_t *i, char **word);
void	process_normal_char(char c, size_t *i, char **word);

/* tokenizer_quotes.c */
int		handle_single_quote(t_shell *shell, size_t *i, char **word,
			t_state *state);
int		handle_double_quote(t_shell *shell, size_t *i, char **word,
			t_state *state);

/* tokenizer_ops.c */
int		is_op_char(char c);
size_t	read_operator(const char *s, t_token **list);

/* expansion.c */
int		handle_variable_expansion(t_shell *shell, size_t *i, char **word);
char	*expand_var(t_shell *shell, size_t *i);

/* expansion_utils.c */
void	append_expansion_quoted(char **word, const char *exp);
void	append_expansion_unquoted(char **word, const char *exp,
			t_token **tokens);

/* continuation.c */
int		append_continuation(char **s, t_state state);

/* parser.c */
void	parse_input(t_shell *shell);

/* add_token_to_cmd.c*/
void	add_token_to_command(t_command *cmd, t_token *token);
void	add_word_to_cmd(t_command *cmd, char *word);

/* parser_syntax_check.c*/
int		syntax_check(t_token *token);
int		syntax_error(const char *msg);

/* argv_build.c*/
void	finalize_all_commands(t_command *cmd);
void	finalize_argv(t_command *cmd);

/* free.c */
void	free_all(t_shell *shell);
void	free_tokens(t_token *token);
void	free_args(t_arg *arg);
void	free_commands(t_command *cmd);

/* heredoc.c */
int			is_heredoc(char *f);
void	process_heredoc(t_command *cmd, char *delimiter);

/* parser */
/* Parse flat token list into commands */
t_command	*parse_tokens(t_token *token);
void	parse_input(t_shell *shell);
t_command	*parse_command(t_token **tokens);
int		handle_redirection(t_command *cmd, t_token **tokens);
int		process_heredocs(t_shell *shell);
int		read_heredoc(t_command *cmd, t_shell *shell);
int		is_redirection(t_tokentype type);
t_builtin	get_builtin_type(char *cmd);
void	free_command(t_command *cmd);
int		is_quoted_delimiter(char *delim);
char	*expand_heredoc_line(char *line, t_shell *shell);

/* executor */
int		execute_commands(t_shell *shell);
int		execute_single_command(t_command *cmd, t_shell *shell);
int		apply_redirections(t_command *cmd);
void	restore_fds(int stdin_backup, int stdout_backup);
int		execute_builtin(t_command *cmd, t_shell *shell);
int		execute_external(t_command *cmd, t_shell *shell);
char	*find_command_path(char *cmd, t_shell *shell);
void	free_array(char **arr);

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
int		readline_event_hook(void);

/* Global signal status (set in signal_handler.c) */
extern volatile sig_atomic_t g_signum;

/* builtin_dispatcher.c */
int		is_builtin(char *cmd);
int		run_builtin(char **argv, t_shell *shell);

/* simple_split.c (for testing) */
char	**simple_split_input(char *input);
void	free_simple_argv(char **argv);

#endif

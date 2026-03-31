/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototypes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:01 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/31 17:28:40 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTOTYPES_H
# define PROTOTYPES_H

/* init.c */
char		*build_prompt(t_shell *shell);
void		init_shell(t_shell *shell, char **envp);
void		process_input(t_shell *shell);
char		*get_env_value(char **envp, const char *key);

/* init_runtime.c */
void		init_runtime_fields(t_shell *shell);

/* utils.c */
char		*ft_strcat(char *dest, const char *src);
char		*ft_realloc(char *ptr, const size_t new_size);
char		**ft_arrdup(char **envp);
void		*msh_calloc(t_shell *shell, const size_t nmemb, const size_t size);
void		clean_exit(t_shell *shell, int exit_status);
char		*msh_strdup(t_shell *shell, char *s);

/* tokenizer.c */
void		tokenize_input(t_shell *shell);

/* tokenizer_utils.c */
void		flush_word(t_shell *shell, char **word, t_token **token);
void		add_token(t_token **head, t_token *new);
t_token		*new_token(t_shell *shell, t_tokentype type, char *value);
int			append_char(t_shell *shell, char **dst, char c);
void		mark_word_quoted(t_shell *shell);
void		set_heredoc_mode(t_shell *shell, int mode);
int			is_heredoc_mode(t_shell *shell);

/* tokenizer_handlers.c */
void		handle_end_of_string(t_shell *shell, t_state *state);
int			handle_backslash(t_shell *shell, size_t *i,
				char **word, t_state *state);
int			process_quote(t_shell *shell, char c, t_state *state);
int			handle_operator(t_shell *shell, size_t *i, char **word);
int			handle_whitespace(t_shell *shell, size_t *i, char **word);
int			process_normal_char(t_shell *shell, char c, size_t *i,
				char **word);

/* tokenizer_quotes.c */
int			handle_single_quote(t_shell *shell, size_t *i,
				char **word, t_state *state);
int			handle_double_quote(t_shell *shell, size_t *i,
				char **word, t_state *state);

/* tokenizer_ops.c */
int			is_op_char(char c);
size_t		read_operator(t_shell *shell, const char *s, t_token **list);

/* expansion.c */
int			handle_variable_expansion(t_shell *shell, size_t *i,
				char **word);
int			handle_tilde_expansion(t_shell *shell, size_t *i, char **word);
char		*expand_var(t_shell *shell, size_t *i);

/* expansion_utils.c */
void		append_expansion_quoted(char **word, const char *exp);
void		append_expansion_unquoted(t_shell *shell, char **word,
				const char *exp, t_token **tokens);
int			handle_empty_unquoted_expansion(t_shell *shell, size_t start,
				size_t end, char **word);

/* continuation.c */
int			append_continuation(t_shell *shell, char **s, t_state state);

/* parser.c */
void		parse_input(t_shell *shell);

/* add_token_to_cmd.c */
int			add_token_to_command(t_shell *shell, t_command *cmd,
				t_token *token);

/* argv_build.c */
void			finalize_all_commands(t_shell *shell, t_command *cmd);

/* parser_syntax_check.c */
int			syntax_check(t_token *token);
int			syntax_error(const char *msg);

/* heredoc.c */
int			process_heredocs(t_shell *shell);
int			read_heredoc(t_command *cmd, t_shell *shell, int *line_no);

/* heredoc_utils.c */
int			is_quoted_delimiter(char *delim);
char		*expand_heredoc_line(char *line, t_shell *shell);

/* free_runtime.c */
void		free_commands(t_command *cmd);
void		free_args(t_arg *arg);

/* free_utils.c */
void		free_tokens(t_token *token);
void		free_array(char **arr);

/* free_shell.c */
void		reset_shell(t_shell *shell);
void		free_all(t_shell *shell);

/* executor.c */
int			execute_commands(t_shell *shell);
int			execute_single_command(t_command *cmd, t_shell *shell);

/* executor_utils.c */
int			apply_redirections(t_command *cmd);
void		restore_fds(int stdin_backup, int stdout_backup);
int			execute_builtin(t_command *cmd, t_shell *shell);
void		set_underscore(t_shell *shell, char *path);

/* executor_external.c */
int			execute_external(t_command *cmd, t_shell *shell);
char		*find_command_path(char *cmd, t_shell *shell);

/* executor_child_exec.c */
void		execute_in_child(t_command *cmd, t_shell *shell);

/* executor_child_format.c */
void		write_err3(char *a, char *b, char *c);
char		*format_cmd_name_for_error(char *cmd_name);

/* executor_pipeline.c */
int			execute_pipeline(t_command *cmds, t_shell *shell);

/* executor_pipeline_not_found.c */
int			handle_all_not_found_pipeline(t_command *cmds, t_shell *shell);

/* builtins */
int			builtin_cd(char **args, t_shell *shell);
int			builtin_pwd(char **args, t_shell *shell);
int			builtin_env(char **args, t_shell *shell);
int			builtin_echo(char **args, t_shell *shell);
int			builtin_export(char **args, t_shell *shell);
int			builtin_unset(char **args, t_shell *shell);
int			builtin_exit(char **args, t_shell *shell);
int			parse_exit_value(char *str, long long *value);

/* export_utils.c */
int			is_valid_export_name(char *name);
int			find_export_key_index(t_shell *shell, char *key, int key_len);
int			append_export_env(t_shell *shell, char *entry);

/* export_print.c */
int			print_sorted_env(t_shell *shell);

/* signal_handler.c */
int			set_signals_default(void);
int			set_signals_ignore(void);
int			set_signals_interactive(void);

/* signal_utils.c */
int			check_signal_received(t_shell *shell);
int			readline_event_hook(void);

/* builtin_dispatcher.c */
int			is_builtin(char *cmd);
int			must_run_in_parent(t_builtin type);
int			run_builtin(char **argv, t_shell *shell);
t_builtin	get_builtin_type(char *cmd);

/* Global signal status */
extern volatile sig_atomic_t	g_signum;

#endif

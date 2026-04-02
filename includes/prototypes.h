/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prototypes.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 14:11:01 by gkhavari          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTOTYPES_H
# define PROTOTYPES_H

/* init.c */
void		init_shell(t_shell *shell, char **envp);
void		process_input(t_shell *shell);

/* init_utils.c */
void		init_shell_identity(t_shell *shell, char **envp);
void		init_runtime_fields(t_shell *shell);
char		*get_env_value(char **envp, const char *key);

/* shell_repl.c */
void		shell_loop(t_shell *shell);

/* ft_strcat.c */
char		*ft_strcat(char *dest, const char *src);

/* ft_arrdup.c */
char		**ft_arrdup(char **envp);

/* ft_realloc.c */
char		*ft_realloc(char *ptr, const size_t new_size);

/* msh_string.c */
int			msh_is_blank(int c, int ifs_mode);
size_t		msh_env_var_body_span(const char *s, size_t start);
int			msh_is_dollar_var_leader(int c);

/* read_stdin_line.c */
int			ft_read_stdin_line(t_shell *shell, char **line,
				int set_shell_oom_on_fail);

/* free_exit.c */
void		clean_exit_before_readline(t_shell *shell, int exit_status);
void		clean_exit(t_shell *shell, int exit_status);

/* tokenizer_loop.c */
int			tokenizer_run_loop(t_shell *shell, size_t *i, t_state *state,
				char **word);

/* tokenizer.c */
int			tokenize_input(t_shell *shell);

/* tokenizer_utils.c */
int			flush_word(t_shell *shell, char **word, t_list **tokens);
int			add_token(t_list **head, t_token *new_tok);
t_token		*new_token(t_shell *shell, t_tokentype type, char *value);
int			append_char(t_shell *shell, char **dst, char c);

/* tokenizer_handlers.c */
void		handle_end_of_string(t_shell *shell, t_state *state, char **word);
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
int			read_operator(t_shell *shell, const char *s, t_list **list);

/* expansion.c */
char		*expand_var(t_shell *shell, size_t *i);

/* expansion_word.c */
int			handle_variable_expansion(t_shell *shell, size_t *i,
				char **word);
int			handle_tilde_expansion(t_shell *shell, size_t *i, char **word);
int			append_expansion_unquoted(t_shell *shell, char **word,
				const char *exp, t_list **tokens);

/* expansion_utils.c */
int			append_expansion_quoted(char **word, const char *exp);
int			handle_empty_unquoted_expansion(t_shell *shell, size_t start,
				size_t end, char **word);

/* parser.c */
void		parse_input(t_shell *shell);
int			process_heredocs(t_shell *shell);

/* parser_build.c */
t_list		*build_command_list(t_shell *shell, t_list *tokens);

/* add_token_to_cmd.c */
int			add_token_to_command(t_shell *shell, t_command *cmd,
				t_list *tok_node);

/* parser_redir.c */
int			parse_redir_token_pair(t_command *cmd, t_list *tok_node);

/* argv_build.c */
int			finalize_all_commands(t_shell *shell, t_list *cmd_list);

/* parser_syntax_check.c */
int			syntax_check(t_list *lst);
int			syntax_error(const char *msg);

/* heredoc_input.c */
char		*heredoc_read_line(t_shell *shell);
void		print_heredoc_eof_warning(int line_no, char *delim);
void		write_heredoc_line(char *line, int fd, int expand, t_shell *shell);

/* heredoc.c */
int			read_heredoc(t_command *cmd, t_shell *shell, int *line_no);

/* heredoc_utils.c */
int			is_quoted_delimiter(char *delim);
char		*expand_heredoc_line(char *line, t_shell *shell);

/* free_runtime.c */
void		free_commands(t_list **cmds);

/* free_utils.c */
void		msh_strptr_free(char **p);
void		free_tokens(t_list **lst);
void		free_args(t_list **lst);

/* free_shell.c */
void		free_lex(t_shell *shell, char **word);
void		reset_shell(t_shell *shell);
void		free_all(t_shell *shell);

/* exe.c — entry */
/** Run parsed commands (single or pipeline); see exe.c */
int			run_commands(t_shell *shell);

/* exe_redir.c */
/** Apply cmd redirs + heredoc stdin; see exe_redir.c */
int			apply_redirs(t_command *cmd);

/* exe_external.c */
/** Fork + wait for one external; see exe_external.c */
int			run_external(t_command *cmd, t_shell *shell);
/** PATH / literal resolve for argv[0]; static buffer; see exe_external.c */
char		*resolve_cmd_path(char *cmd, t_shell *shell);

/* exe_not_found.c */
/** Stderr "command not found" line; see exe_not_found.c */
void		put_cmd_not_found(char *cmd_name);

/* exe_child.c */
/** Post-fork child: builtin, execve, or errors; see exe_child.c */
void		run_in_child(t_command *cmd, t_shell *shell);

/* exe_pipeline_nf.c */
/** Pipeline all-stages not-found fast path; see exe_pipeline_nf.c */
int			pipeline_all_nf(t_list *cmds, t_shell *shell);

/* exe_pipeline.c */
/** Run command list as a pipeline; see exe_pipeline.c */
int			run_pipeline(t_list *cmds, t_shell *shell);

/* exe_pipe_step.c */
/** One pipeline fork + pipe wiring; see exe_pipe_step.c */
pid_t		pipe_step(t_list *cmd_node, t_shell *shell,
				int *prev_fd, int sync_fd[2]);

/* builtins */
int			builtin_cd(char **args, t_shell *shell);
int			builtin_pwd(char **args, t_shell *shell);
int			builtin_env(char **args, t_shell *shell);
int			builtin_echo(char **args, t_shell *shell);
int			builtin_export(char **args, t_shell *shell);
int			builtin_unset(char **args, t_shell *shell);
int			builtin_exit(char **args, t_shell *shell);

/* exit_utils.c */
int			parse_exit_value(char *str, long long *value);

/* export_utils.c */
void		sort_env_strings(char **sorted, int count);
void		export_invalid_identifier_err(const char *arg);
int			is_valid_export_name(char *name);
int			find_export_key_index(t_shell *shell, char *key, int key_len);
int			append_export_env(t_shell *shell, char *entry);

/* export_print.c */
int			print_sorted_env(t_shell *shell);

/* signal_handler.c (install, g_signum) */
int			set_signals_default(void);
int			set_signals_ignore(void);
int			set_signals_interactive(void);

/* signal_utils.c (readline hook consumes g_signum) */
int			check_signal_received(t_shell *shell);
int			readline_event_hook(void);

/* builtin_dispatcher.c */
int			run_builtin(char **argv, t_shell *shell);
t_builtin	get_builtin_type(char *cmd);

/* Global signal status */
extern volatile sig_atomic_t	g_signum;

#endif

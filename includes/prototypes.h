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

/* --- init_env.c --- */
/** Return value for KEY in envp (KEY=value), or NULL. */
char		*get_env_value(char **envp, const char *key);
/** Dup envp, USER, PWD into shell; fatal on allocation failure. */
void		init_shell_identity(t_shell *shell, char **envp);
/** Default env keys, last_exit, null tokens/cmds/input, runtime flags. */
void		init_runtime_fields(t_shell *shell);

/* --- init_shell.c --- */
/** Allocate env copy, user, cwd; bump SHLVL on TTY. */
void		init_shell(t_shell *shell, char **envp);

/* --- repl_process.c --- */
/** Tokenize, parse, heredocs, then run commands for one line. */
void		process_input(t_shell *shell);

/* --- repl_loop.c --- */
/** Interactive or non-interactive read-eval loop until EOF. */
void		shell_loop(t_shell *shell);

/* --- msh_string_append_char.c --- */
/** Append one byte to *dst; OOM frees *dst. */
int			append_char(t_shell *shell, char **dst, char c);

/* --- msh_word_append_expanded.c --- */
/** Concatenate exp onto *word; OOM on realloc failure. */
int			exp_q_cat(char **word, const char *exp);

/* --- msh_strarray_dup.c --- */
/** Deep copy of NULL-terminated string array; NULL on error. */
char		**ft_arrdup(char **envp);

/* --- msh_char_buffer_realloc.c --- */
/** Reallocate buffer to new_size bytes; frees ptr on failure. */
char		*ft_realloc(char *ptr, const size_t new_size);

/* --- msh_string_expand_scan.c --- */
/** True if c is IFS whitespace (or space/tab when ifs_mode). */
int			msh_is_blank(int c, int ifs_mode);
/** Byte length of env var name body in s starting at start (after $). */
size_t		msh_env_var_body_span(const char *s, size_t start);
/** True if c may start a braced or plain parameter expansion. */
int			msh_is_dollar_var_leader(int c);

/* --- msh_stdin_read_line.c --- */
/** Read stdin until newline or EOF; RL_LN, RL_EOF, or OOM. */
int			ft_read_stdin_line(t_shell *shell, char **line,
				int set_shell_oom_on_fail);

/* --- free_exit.c --- */
/** Exit without readline cleanup (child process or init fatal path). */
void		exit_norl(t_shell *shell, int exit_status);
/** Exit builtin path: free shell and readline cleanup. */
void		clean_exit(t_shell *shell, int exit_status);

/* --- tokenizer_loop.c --- */
/** Per-character tokenizer loop over input; updates *i and *word. */
int			tokenizer_loop(t_shell *shell, size_t *i, t_state *state,
				char **word);

/* --- tokenizer.c --- */
/** Build token list from shell->input; sets last_exit on hard errors. */
int			tokenize_input(t_shell *shell);

/* --- tokenizer_utils.c --- */
/** Flush *word as WORD token; clear *word. */
int			flush_word(t_shell *shell, char **word, t_list **tokens);
/** Append new_tok to token list; OOM on failure. */
int			add_token(t_list **head, t_token *new_tok);
/** Allocate token; strdup value; NULL on failure. */
t_token		*new_token(t_shell *shell, t_tokentype type, char *value);
/** Append literal char in ST_NORMAL; advance *i. */
int			process_normal_char(t_shell *shell, char c, size_t *i,
				char **word);

/* --- tokenizer_handlers.c --- */
/** At end of input: flush word and signal stop. */
void		handle_end_of_string(t_shell *shell, t_state *state, char **word);
/** Handle backslash depending on quote state. */
int			handle_backslash(t_shell *shell, size_t *i,
				char **word, t_state *state);
/** Toggle ST_NORMAL / SQUOTE / DQUOTE on quote char. */
int			process_quote(t_shell *shell, char c, t_state *state);
/** Lex pipe or redirection at *i. */
int			handle_operator(t_shell *shell, size_t *i, char **word);
/** End current word at whitespace. */
int			handle_whitespace(t_shell *shell, size_t *i, char **word);

/* --- tokenizer_quotes.c --- */
/** Single-quoted segment: literals until closing '. */
int			handle_single_quote(t_shell *shell, size_t *i,
				char **word, t_state *state);
/** Double-quoted segment: $ expansion and literals until closing ". */
int			handle_double_quote(t_shell *shell, size_t *i,
				char **word, t_state *state);

/* --- tokenizer_ops.c --- */
/** True if c begins an operator token. */
int			is_op_char(char c);
/** Parse operator at s; append tokens to list. */
int			read_operator(t_shell *shell, const char *s, t_list **list);

/* --- expansion.c --- */
/** Expand $name, $?, or ${...}; advance *i; caller frees return. */
char		*exp_var(t_shell *shell, size_t *i);

/* --- expansion_word.c --- */
/** Unquoted: expand $ at *i into *word. */
int			exp_dollar(t_shell *shell, size_t *i, char **word);
/** Unquoted: expand leading ~ or ~user at *i into *word. */
int			exp_tilde(t_shell *shell, size_t *i, char **word);
/** Split unquoted expansion into WORD tokens (field splitting). */
int			exp_unq(t_shell *shell, char **word, const char *exp,
				t_list **tokens);

/* --- expansion_utils.c --- */
/** Insert S_EMPTY placeholder when expansion yields empty word. */
int			exp_empty(t_shell *shell, size_t start, size_t end,
				char **word);

/* --- parse_input.c --- */
/** Syntax-check tokens, parse to cmds, finalize; frees tokens. */
void		parse_input(t_shell *shell);

/* --- parse_syntax.c --- */
/** Validate pipe and redirection syntax on token list. */
int			syntax_check(t_list *lst);
/** Print unexpected-token message; return FAILURE. */
int			syntax_error(const char *msg);

/* --- parse_pipeline.c --- */
/** Build pipeline (`t_list` of `t_command`) from token list. */
t_list		*build_command_list(t_shell *shell, t_list *tokens);

/* --- parse_attach_token.c --- */
/** Attach next tokens to cmd: args, redirs, heredoc delimiter. */
int			add_token_to_command(t_shell *shell, t_command *cmd,
				t_list *tok_node);

/* --- parse_redir.c --- */
/** Parse redirection token plus following WORD. */
int			parse_redir_token_pair(t_command *cmd, t_list *tok_node);

/* --- parse_finalize.c --- */
/** For each command: build argv, set is_builtin; OOM on failure. */
int			finalize_cmds(t_shell *shell, t_list *cmd_list);

/* --- heredoc_collect.c --- */
/** Read lines until delimiter; sets cmd->hd_fd read end. */
int			read_heredoc(t_command *cmd, t_shell *shell, int *line_no);
/** Read heredoc bodies for all commands; updates last_exit on error. */
int			process_heredocs(t_shell *shell);

/* --- heredoc_io.c --- */
/** Read one heredoc line (readline or stdin). */
char		*heredoc_read_line(t_shell *shell);
/** Warn when heredoc ends at EOF before delimiter. */
void		print_heredoc_eof_warning(int line_no, char *delim);
/** Write line to heredoc pipe; optional expansion. */
void		write_heredoc_line(char *line, int fd, int expand, t_shell *shell);

/* --- heredoc_expand.c --- */
/** True if delimiter token had quotes (suppress expansion). */
int			is_quoted_delimiter(char *delim);
/** Parameter-expand one heredoc line when allowed. */
char		*exp_hd_line(char *line, t_shell *shell);

/* --- free_runtime.c --- */
/** Free a NULL-terminated string array and each element (argv, envp). */
void		free_argv(char **argv);
/** Free pipeline command list and nested data. */
void		free_cmds(t_list **lst);

/* --- free_utils.c --- */
/** Free *p and set pointer to NULL. */
void		msh_strptr_free(char **p);
/** Free all lexer tokens. */
void		free_tokens(t_list **lst);
/** Free all argument nodes for one command. */
void		free_args(t_list **lst);

/* --- free_shell.c --- */
/** After tokenizer OOM: free word, tokens, input. */
void		free_tokenize(t_shell *shell, char **word);
/** Between prompts: free tokens, cmds, input; keep env. */
void		reset_shell(t_shell *shell);
/** At exit: free env, user, cwd, tokens, cmds, input. */
void		free_all(t_shell *shell);

/* --- exec_redir.c --- */
/** Apply redirects and heredoc stdin wiring for one command. */
int			apply_redirs(t_command *cmd);

/* --- exec_notfound.c --- */
/** Print the command-not-found line to stderr. */
void		put_cmd_not_found(char *cmd_name);

/* --- exec_external.c --- */
/** Fork and wait for one external command. */
int			run_external(t_command *cmd, t_shell *shell);
/** Resolve argv[0] via PATH or literal path; static buffer. */
char		*resolve_cmd_path(char *cmd, t_shell *shell);

/* --- exec_child.c --- */
/** Child process: builtin, execve, or error exit. */
void		run_in_child(t_command *cmd, t_shell *shell);

/* --- exec_pipeline_nf.c --- */
/** Pipeline fast path when every stage is not found. */
int			pip_all_nf(t_list *cmds, t_shell *shell);

/* --- exec_pipe_step.c --- */
/** Fork one pipeline stage: wire prev_fd/stdout, optional sync-fd read. */
pid_t		pipe_step(t_list *cmd_node, t_shell *shell,
				int *prev_fd, int sync_fd[2]);

/* --- exec_pipeline.c --- */
/** Run a command list as a pipeline. */
int			run_pip(t_list *cmds, t_shell *shell);

/* --- exec_dispatch.c --- */
/** Run parsed commands (single command or pipeline). */
int			run_commands(t_shell *shell);

/* --- builtins --- */
/** Builtin cd: HOME, OLDPWD (-), or path. */
int			builtin_cd(char **args, t_shell *shell);
/** Builtin pwd: print cwd. */
int			builtin_pwd(char **args, t_shell *shell);
/** Builtin env: print environment. */
int			builtin_env(char **args, t_shell *shell);
/** Builtin echo: -n handling. */
int			builtin_echo(char **args, t_shell *shell);
/** Builtin export: set or list exported vars. */
int			builtin_export(char **args, t_shell *shell);
/** Builtin unset: remove env vars. */
int			builtin_unset(char **args, t_shell *shell);
/** Builtin exit: numeric status and arg checks. */
int			builtin_exit(char **args, t_shell *shell);

/* --- exit_utils.c --- */
/** Parse exit argument into *value; 0 if invalid or overflow. */
int			parse_exit_value(char *str, long long *value);

/* --- export_utils.c --- */
/** Sort env string array (e.g. for export -p). */
void		sort_env_strings(char **sorted, int count);
/** Print invalid identifier error for export. */
void		export_invalid_identifier_err(const char *arg);
/** True if name is valid for export. */
int			is_valid_export_name(char *name);
/** Index of key= in shell envp, or -1. */
int			find_export_key_index(t_shell *shell, char *key, int key_len);
/** Append one KEY=value line to envp; realloc. */
int			append_export_env(t_shell *shell, char *entry);

/* --- export_print.c --- */
/** Print sorted env as `export name=value` lines. */
int			print_sorted_env(t_shell *shell);

/* --- signal_handler.c --- */
/** Child/exec: default SIGINT, SIGQUIT, SIGPIPE, SIGTERM. */
int			set_signals_default(void);
/** Parent waiting on child: ignore SIGINT and SIGQUIT. */
int			set_signals_ignore(void);
/** Interactive readline: custom SIGINT; ignore QUIT/TERM/SIGPIPE. */
int			set_signals_interactive(void);

/* --- signal_utils.c --- */
/** If SIGINT pending, set last_exit XSINT and clear flag. */
int			check_signal_received(t_shell *shell);
/** Readline hook: clear line when SIGINT was received. */
int			readline_event_hook(void);

/* --- builtin_dispatcher.c --- */
/** Run matching builtin from argv; return exit status. */
int			run_builtin(char **argv, t_shell *shell);
/** Map argv[0] to builtin id or B_NONE. */
t_builtin	get_builtin_type(char *cmd);

/** Last signal received (SIGINT); async-safe. */
extern volatile sig_atomic_t	g_signum;

#endif

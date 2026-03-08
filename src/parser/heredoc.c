/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:56:43 by gkhavari          #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void write_heredoc_line(char *line, int fd, int expand, t_shell *shell) {
  char *expanded;

  if (expand) {
    expanded = expand_heredoc_line(line, shell);
    ft_putendl_fd(expanded, fd);
    free(expanded);
  } else
    ft_putendl_fd(line, fd);
}

static int heredoc_check_delim(char *line, t_command *cmd, int *pipe_fd) {
  if (ft_strcmp(line, cmd->heredoc_delim) == 0) {
    free(line);
    close(pipe_fd[1]);
    cmd->heredoc_fd = pipe_fd[0];
    return (1);
  }
  return (0);
}

/*
** read_heredoc_line - Read one line for heredoc input
** In interactive mode: use readline with "> " prompt.
** In non-interactive (tester/script) mode: use raw read() to avoid
** readline's internal buffer conflicting with the REPL loop's readline.
*/
static char *read_heredoc_line(void) {
  char buf[1];
  char *line;
  size_t len;
  ssize_t n;

  if (isatty(STDIN_FILENO))
    return (readline("> "));
  line = ft_strdup("");
  if (!line)
    return (NULL);
  len = 0;
  while (1) {
    n = read(STDIN_FILENO, buf, 1);
    if (n <= 0) {
      if (len == 0)
        return (free(line), NULL);
      return (line);
    }
    if (buf[0] == '\n')
      return (line);
    line = ft_realloc(line, len + 2);
    if (!line)
      return (NULL);
    line[len++] = buf[0];
    line[len] = '\0';
  }
}

static int heredoc_read_loop(t_command *cmd, t_shell *shell, int *pipe_fd,
                             int expand) {
  char *line;

  while (1) {
    line = read_heredoc_line();
    if (g_signum == SIGINT)
      return (free(line), close(pipe_fd[0]), close(pipe_fd[1]), 1);
    if (!line) {
      ft_putstr_fd("minishell: warning: here-document delimited"
                   " by EOF\n",
                   2);
      break;
    }
    if (heredoc_check_delim(line, cmd, pipe_fd))
      return (0);
    write_heredoc_line(line, pipe_fd[1], expand, shell);
    free(line);
  }
  close(pipe_fd[1]);
  cmd->heredoc_fd = pipe_fd[0];
  return (0);
}

int read_heredoc(t_command *cmd, t_shell *shell) {
  int pipe_fd[2];
  int expand;

  if (pipe(pipe_fd) == -1)
    return (1);
  expand = !cmd->heredoc_quoted;
  return (heredoc_read_loop(cmd, shell, pipe_fd, expand));
}

int process_heredocs(t_shell *shell) {
  t_command *cmd;

  cmd = shell->commands;
  while (cmd) {
    if (cmd->heredoc_delim) {
      if (read_heredoc(cmd, shell))
        return (1);
    }
    cmd = cmd->next;
  }
  return (0);
}

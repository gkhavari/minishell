/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkhavari <gkhavari@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 21:56:43 by gkhavari          #+#    #+#             */
/*   Updated: 2025/12/08 21:56:46 by gkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
DESCRIPTION:
 * Checks whether a given filename corresponds to a minishell-generated
 	heredoc file.

PARAMETERS:
* f: File path string to test.

RETURN:
* 1: if the filename starts with the heredoc prefix.
* 0: otherwise.
 **/
int	is_heredoc(char *f)
{
	return (ft_strncmp(f, "/tmp/.minishell_heredoc_", 25) == 0);
}

/**
DESCRIPTION:
* Generates a unique filename for storing heredoc content.
* Each call increments a static counter and returns a path formatted as:
	/tmp/.minishell_heredoc_<counter>

RETURNS:
* A newly allocated string containing the unique path.
* NULL if allocation fails.

BEHAVIOR:
* Maintains a static int counter that increments on each call.
* Converts the counter to a string (ft_itoa).
* Concatenates it to the heredoc filename prefix.
**/
char	*heredoc_filename(void)
{
	static int	counter = 0;
	char		*num;
	char		*name;

	num = ft_itoa(counter++);
	name = ft_strjoin("/tmp/.minishell_heredoc_", num);
	free(num);
	return (name);
}

/**
DESCRIPTION:
* Creates a heredoc temporary file, reads lines from the user until the 
	delimiter is encountered, and stores the resulting file path in
	cmd->input_file.
* This function handles the full heredoc workflow:
** Generate a unique temporary filename.
** Open the file for exclusive write.
** Repeatedly prompt the user ("> ") using readline().
** Stop when the line matches the delimiter or EOF occurs.
** Write all entered lines into the temporary file.
** Attach the created file as the command’s input file.

PARAMETERS:
* cmd: The command to assign the generated heredoc file to.
* delimiter: The stop string;

BEHAVIOR:
* Calls heredoc_filename() to obtain a unique file path.
* Opens the file
* In a loop:
** Reads a line with readline("> ").
** Breaks on:
*** NULL (EOF, Ctrl+D)
*** Exact string match to the delimiter
** Writes the line plus a newline to the file.
* Frees any previously assigned cmd->input_file.
* Assigns the new path to cmd->input_file.
**/
void	process_heredoc(t_command *cmd, char *delimiter)
{
	char	*line;
	char	*path;
	int		fd;

	path = heredoc_filename();
	if (!path)
		return ;
	fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0600);
	if (fd < 0)
	{
		perror("heredoc");
		free(path);
		return ;
	}
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0) // if delimiter is "" then delimiter is '\n' (changes in syntax check also needed)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	close(fd);
	if (cmd->input_file)
		free(cmd->input_file);
	cmd->input_file = path;
}

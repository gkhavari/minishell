#include "minishell.h"

int	is_heredoc(char *f)
{
    return (ft_strncmp(f, "/tmp/.minishell_heredoc_", 25) == 0);
}

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
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
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

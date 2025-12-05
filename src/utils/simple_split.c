/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thanh-ng <thanh-ng@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 21:35:00 by thanh-ng          #+#    #+#             */
/*   Updated: 2025/12/03 21:15:01 by thanh-ng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Free argv array (also defined in free.c, using here just for testing)
*/
static void	free_split_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

/*
** Simple split for testing builtins
** Splits input by spaces into argv array
** Does NOT handle quotes properly - just for basic testing
** Returns: NULL-terminated array of strings
*/
char	**simple_split_input(char *input)
{
	char	**argv;
	int		i;
	int		j;

	if (!input || !*input)
		return (NULL);
	while (*input && (*input == ' ' || *input == '\t'))
		input++;
	if (!*input)
		return (NULL);
	argv = ft_split(input, ' ');
	if (!argv)
		return (NULL);
	i = 0;
	j = 0;
	while (argv[i])
	{
		if (argv[i][0] != '\0')
			argv[j++] = argv[i];
		else
			free(argv[i]);
		i++;
	}
	argv[j] = NULL;
	return (argv);
}

/*
** Free argv array - wrapper for external use
*/
void	free_simple_argv(char **argv)
{
	free_split_argv(argv);
}

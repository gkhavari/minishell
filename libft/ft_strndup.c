#include "libft.h"

char	*ft_strndup(const char *s, size_t n)
{
	size_t	i;
	size_t	j;
	char	*dup;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && i < n)
		i++;
	dup = malloc(i + 1);
	if (!dup)
		return (NULL);
	j = 0;
	while (j < i)
	{
		dup[j] = s[j];
		j++;
	}
	dup[i] = '\0';
	return (dup);
}
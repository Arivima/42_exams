#include "get_next_line.h"

int	ft_strlen(char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_newline(char *s)
{
	int i = 0;
	while (s[i])
	{
		if (s[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char *s;
	int i, j;

	if (!s1 || !s2)
		return (NULL);
	if (!(s = (char *) malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1))))
		return (NULL);

	i = 0;
	while (s1[i])
	{
		s[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		s[i + j] = s2[j];
		j++;
	}
	s[i + j] = 0;
	return (s);
}

char	*ft_substr(char *s, int start, int end)
{
	char	*new;;
	int		i;

	if (!s || start >= ft_strlen(s) || start < 0 || end < start)
		return (NULL);
	if (!(new = (char *) malloc(sizeof(char) * (end - start + 2))))
		return (NULL);

	i = 0;
	while (s[start + i] && (start + i) <= end)
	{
		new[i] = s[start + i];
		i++;
	}
	new[i] = 0;
	return (new);
}

char	*get_next_line(int fd)
{
	char		buf[BUFFER_SIZE + 1];
	static char	*s;
	char		*tmp;
	char		*line;
	int			n, i;

	if (BUFFER_SIZE <= 0 || fd < 0 || (read(fd, buf, 0) < 0))
		return (NULL);
	if (!s)
	{
		if (!(s = (char *) malloc(sizeof(char))))
			return (NULL);
		s[0] = 0;
	}
	
	tmp = 0;
	line = 0;
	n = 0;
	i = 0;

	if ((i = ft_newline(s)) < 0)
	{
		while ((n = read(fd, buf, BUFFER_SIZE)))
		{
			buf[n] = 0;
			tmp = ft_strjoin(s, buf);
			free(s);
			s = tmp;
			if ((i = ft_newline(s)) >= 0)
				break;
		}
	}
	if (i >= 0)
	{
		line = ft_substr(s, 0, i);
		tmp = ft_substr(s, i + 1, ft_strlen(s));
		free(s);
		s = tmp;
		return (line);
	}
	else
	{
		line = ft_substr(s, 0, ft_strlen(s));
		free(s);
		s = 0;
		if (line)
			return (line);
		else
			return (NULL);
	}
	return (NULL);
}


#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>

int	main()
{
	
	// char	*s = "Hello \nworld !";
	// char	*s2 = "Coucou";
	// int		start = 0;
	// int		end  = ft_strlen(s) + 5;
	// char	buf[BUFFER_SIZE + 1];
	// int		n = 0;
	// int		fd = open("hp.txt", O_RDONLY);

	// printf("---------------------------------\n");
	// printf("s : %s\ns2 : %s\n", s, s2);
	// printf("---------------------------------\n");
	// printf("ft_strlen(s) : %d\n(%s)\n", ft_strlen(s), s);
	// printf("---------------------------------\n");
	// printf("ft_newline(s) : %d\n(%s)\n", ft_newline(s), s);
	// printf("---------------------------------\n");
	// printf("ft_strjoin(s, s2) : %s\n(s : %s; s2 : %s)\n", ft_strjoin(s, s2), s, s2);
	// printf("---------------------------------\n");
	// printf("ft_substr(s, x, y) : %s\n(s : %s)\n(ft_strlen(s) : %d; start : %d; end %d)\n", ft_substr(s, start, end), s, ft_strlen(s), start, end);
	// printf("---------------------------------\n");
	// n = read(fd, buf, BUFFER_SIZE);
	// buf[n] = 0;
	// printf("n read : %d | fd : %d | BUF_S : %d\ntext : %s", n, fd, BUFFER_SIZE, buf);


	int		fd = open("big.txt", O_RDONLY);
	printf("%d\n", fd);
	if (fd < 0)
		perror("open");
	char	*line;

	line = 0;
	printf(">%p\n|| %s\n", line, line);
	
	line = get_next_line(fd);
	while (line)
	{
		printf(">%p\n|| %s", line, line);
		free(line);
		line = 0;
		line = get_next_line(fd);

	}
	printf("end\n>%p\n|| %s", line, line);
	free(line);
	line = 0;
	return (0);

}
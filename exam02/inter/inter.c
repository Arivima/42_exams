#include <unistd.h>

int main(int ac, char ** av)
{
	char 	used[200];
	int		i;

	if (ac == 3)
	{
		i = 0;
		while (i < 200)
			used[i++] = 0;
		i = 0;
		while (av[2][i])
		{
			used[(unsigned char) av[2][i]] = 1;
			i++;
		}
		i = 0;
		while (av[1][i])
		{
			if (used[(unsigned char) av[1][i]] == 1)
			{
				write(1, &av[1][i], 1);
				used[(unsigned char) av[1][i]] = 0;
			}
			i++;
		}
	}
	write(1, "\n", 1);
	return (0);
}
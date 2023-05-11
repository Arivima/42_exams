#include <unistd.h>

int main(int ac, char ** av)
{
	char	used[200];
	int		i, j;

	if (ac == 3)
	{
		i = 0;
		while (i < 200)
			used[i++] = 0;
		i = 1;
		while (i < 3)
		{
			j = 0;
			while (av[i][j])
			{
				if (used[(unsigned char) av[i][j]] == 0)
				{
					used[(unsigned char) av[i][j]] = 1;
					write(1, &av[i][j], 1);
				}
				j++;
			}
			i++;
		}
	}
	write(1, "\n", 1);
	return (0);
}
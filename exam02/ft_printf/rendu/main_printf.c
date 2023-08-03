#include <stdio.h>

int ft_printf(const char *, ... );

int	main()
{
	char	*s = "Hello";
	int		n = 42;
	int		i;

	i = printf("___printf || s : %s | int : %d | hex : %x ||", s, n, n);
	printf("i = %d\n", i);
	i = ft_printf("ft_printf || s : %s | int : %d | hex : %x ||", s, n, n);
	printf("i = %d\n", i);
	return (0);
}
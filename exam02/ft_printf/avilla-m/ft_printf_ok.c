#include <unistd.h>
#include <stdarg.h>

typedef struct s_struct
{
	int	i;
}				t_struct;

void	ft_putchar(char c, t_struct *count)
{
	write(1, &c, 1);
	count->i++;
}

void	ft_putstr(char *s, t_struct *count)
{
	if (!s)
		s = "(null)";
	while (*s)
		ft_putchar(*(s++), count);
}

void	ft_putnbr_base(unsigned int baselen, char *base, unsigned int nb, t_struct *count)
{
	if (nb >= baselen)
	{
		ft_putnbr_base(baselen, base, nb / baselen, count);
		ft_putnbr_base(baselen, base, nb % baselen, count);
	}
	else
		ft_putchar(base[nb], count);
}

void	ft_print_int(int nb, t_struct *count)
{
	if (nb < 0)
	{
		nb *= -1;
		ft_putchar('-', count);
	}
	ft_putnbr_base(10, "0123456789", nb, count);
}


void	parse_print(char c, va_list av, t_struct *count)
{
	if (c == 's')
		ft_putstr(va_arg(av, char *), count);
	if (c == 'd' || c == 'i')
		ft_print_int(va_arg(av, int), count);
	if (c == 'x')
		ft_putnbr_base(16, "0123456789abcdef", va_arg(av, unsigned int), count);
}

int ft_printf(const char *s, ... )
{
	va_list		av;
	t_struct	count;

	va_start(av, s);
	count.i = 0;
	while (*s)
	{
		if (*s == '%' && *(s++))
			parse_print(*(s++), av, &count);
		else
			ft_putchar(*(s++), &count);
	}
	va_end(av);
	return (count.i);
}


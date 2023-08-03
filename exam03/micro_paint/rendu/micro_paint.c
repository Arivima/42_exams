# include "micro_paint.h"

int	ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_perror(char *err)
{
	write(1, err, ft_strlen(err));
	return (1);
}

int	is_in_rect(float xi, float yi, float sx, float sy, float w, float h)
{
	if ((xi >= sx && xi <= sx + w) && (yi >= sy && yi <= sy + h))
	{
		if (xi - sx < 1.0000000 || yi - sy < 1.0000000 || sx + w - xi < 1.0000000 || sy + h - yi < 1.0000000)
			return (1);
		return (2);
	}
	return (0);
}


int	main(int ac, char **av)
{
	FILE	*file;
	char	background;
	char	*canvas;
	char	r_R, color;
	int		zone_width, zone_height, xi, yi, n, read;
	float	sx, sy, w, h;

	if (ac == 2)
	{
		if (!(file = fopen(av[1], "r"))
			|| (fscanf(file, "%d %d %c\n", &zone_width, &zone_height, &background) != 3)
			|| (zone_width <= 0 || zone_width > 300 || zone_height <= 0 || zone_height > 300)
			|| !(canvas = (char *) malloc(sizeof(char) * ((zone_height * zone_width) + 1)))
			)
			return(ft_perror(ERR2));
		memset(canvas, background, zone_height * zone_width);
		while ((read = fscanf(file, "%c %f %f %f %f %c\n", &r_R, &sx, &sy, &w, &h, &color)) == 6)
		{
			if ((w < 0 || h < 0 || !(r_R == 'r' || r_R == 'R')))
				break;
			yi = -1;
			while (++yi < zone_height)
			{
				xi =  -1;
				while (++xi < zone_width)
				{
					n = is_in_rect((float) xi, (float) yi, sx, sy, w, h);
					if (n == 1 || (n == 2 && r_R == 'R'))
						canvas[yi * zone_width + xi] = color;
				}
			}
		}
		if (read != -1)
		{
			free(canvas);
			return (ft_perror(ERR2));
		}
		yi = -1;
		while (++yi < zone_height)
		{
			write(1, (canvas + (yi * zone_width)), zone_width);
			write(1, "\n", 1);
		}
		free(canvas);
		fclose(file);
	}
	else
		return (ft_perror(ERR1));
	return (0);
}
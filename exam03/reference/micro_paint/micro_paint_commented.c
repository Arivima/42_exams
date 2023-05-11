#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define ERR1 "Error: argument\n"
#define ERR2 "Error: Operation file corrupted\n"

int ft_strlen(char *str)
{
	int	idx = 0;
	while (str[idx])
		idx++;
	return (idx);
}

int	ft_perror(char *str)
{
	write(1, str, ft_strlen(str));
	return (1);
}

int	check_pos(float x, float y, float centre_x, float centre_y, float width, float height)
{
	if (x < centre_x || x > centre_x + width || y < centre_y || y > centre_y + height)
		return (0); //outside
	else if (x - centre_x < 1.0000000 || (centre_x + width) - x < 1.0000000 ||
			y - centre_y < 1.0000000 || (centre_y + height) - y < 1.0000000) // border
		return (1);
	return (2); // inside of square
}

int	main(int argc, char *argv[])
{
	FILE	*file;
	char	*canvas;
	int		read, pos;
	int		x, y;

	int		b_width, b_height;
	char	background;

	char	id, color;
	float	centre_x, centre_y, width, height;

	if (argc != 2) // without argument
		return (ft_perror(ERR1)); // Error 1 output, function termination
	if (!(file = fopen(argv[1], "r")) || // The file of the first argument cannot be opened, or
		(fscanf(file, "%d %d %c\n", &b_width, &b_height, &background) != 3) || // The first line of the file does not match the condition or
		(!(b_width > 0 && b_width <= 300 && b_height > 0 && b_height <= 300)) || // Or the background is not within the specified range
		(!(canvas = (char *)malloc(sizeof(char) * (b_width * b_height))))) // If you cannot allocate as much memory as the background
		return (ft_perror(ERR2)); // Error 2 output, function termination
	memset(canvas, background, b_width * b_height); // Background drawing
	while ((read = fscanf(file, "%c %f %f %f %f %c\n", &id, &centre_x, &centre_y, &width, &height, &color)) == 6) // Repeat if the condition (format, number) is met from the second line of the file
	{
		if (!(width > 0 && height > 0) || !(id == 'R' || id == 'r')) // If the size to be drawn is negative or the id is not'R' or'r'
				break ; // Loop escape
		y = -1;
		while (++y < b_height) // Repeat until the background height
		{
			x = -1;
			while (++x < b_width) // Repeat until background width
			{
				pos = check_pos((float)x, (float)y, centre_x, centre_y, width, height); // Boundary check
				if (pos == 1 || (pos == 2 && id == 'R'))
					canvas[y * b_width + x] = color; // drawing
			}
		}
	}
	if (read != -1) // What if I escaped the loop and the reason was not that the reading failed Because it doesn’t meet the conditions
	{
		free(canvas); // Release background memory
		return (ft_perror(ERR2)); // Error 2 output, function termination
	}
	y = -1;
	while (++y < b_height) // Repeat to background height
	{
		write(1, canvas + y * b_width, b_width); // Painting
		write(1, "\n", 1); // Newline
	}
	free(canvas); // Release background memory
	fclose(file); // Close file
	return (0); // Function termination
}

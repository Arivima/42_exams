#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ERR1 "Error: argument\n"
#define ERR2 "Error: Operation file corrupted\n"

int ft_strlen(char *str)
{
	int	idx = 0;
	while (str[idx])
		idx++;
	return (idx);
}

int	ft_perror(char *msg)
{
	write(1, msg, ft_strlen(msg));
	return (1);
}

int	check_pos(float x, float y, float centre_x, float centre_y, float radius)
{
	float distance = sqrtf(powf(x - centre_x, 2.) + powf(y - centre_y, 2.));
	if (distance <= radius)
	{
		if ((radius - distance) < 1.00000000)
			return (0); // border circle
		return (1); //inside circle
	}
	return (2); //outside circle
}

int	main(int argc, char *argv[])
{
	FILE	*file;
	char	*canvas; // the array
	int		read, pos;
	int		x, y; // iterators

	int		b_width, b_height; // background filler for width and height
	char	background;

	char	id, color; // char to print for cricle (either outline of filled based on C or c)
	float	centre_x, centre_y, radius; // centre-x, centre-y pos

	if (argc != 2) // Without argument
		return (ft_perror(ERR1)); // Error 1 output, function termination
	if (!(file = fopen(argv[1], "r")) || // The file of the first argument cannot be opened, or
		(fscanf(file, "%d %d %c\n", &b_width, &b_height, &background) != 3) || // The first line of the file does not match the condition or
		(!(b_width > 0 && b_width <= 300 && b_height > 0 && b_height <= 300)) || // Or the background is not within the specified range
		(!(canvas = (char *)malloc(sizeof(char) * (b_width * b_height))))) // If you cannot allocate as much memory as the background
		return (ft_perror(ERR2)); // Error 2 output, function termination
	memset(canvas, background, b_width * b_height); // Background drawing
	while ((read = fscanf(file, "%c %f %f %f %c\n", &id, &centre_x, &centre_y, &radius, &color)) == 5) // Repeat if the condition (format, number) is met from the second line of the file
	{
		if (!(radius > 0) || !(id == 'C' || id == 'c')) // If the radius value is negative or the id is not'C' or'c'
			break ; // Loop escape
		y = -1;
		while (++y < b_height) // Repeat to background height
		{
			x = -1;
			while (++x < b_width) // Repeat until background width
			{
				pos = check_pos((float)x, (float)y, centre_x, centre_y, radius); // Boundary check
				if (pos == 0 || (pos == 1 && id == 'C'))
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
		write(1, "\n", 1); // 개행
	}
	free(canvas); // Release background memory
	fclose(file); // Close file
	return (0); // Function termination
}

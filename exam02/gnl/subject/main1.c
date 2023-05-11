#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get_next_line.h"

int     main(void) {
    char *line;

    line = get_next_line(0);
    while (line != NULL) {
        write(1, line, strlen(line));
        free(line);
        line = get_next_line(0);
    }
    return (0);
}

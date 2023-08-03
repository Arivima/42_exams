#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "get_next_line.h"

static void reader(int fd)
{
    char *line;

    if (dup2(fd, 0) < 0)
    {
        write(2, "DUP2 ERROR\n", strlen("DUP2 ERROR\n"));
        return;
    };
    line = get_next_line(0);
    while (line != NULL) {
        write(1, line, strlen(line));
        free(line);
        line = get_next_line(0);
    }
    close(fd);
}

static void writer(int fd)
{
    char buffer[1000];
    int i;
    int j;

    j = 1;
    while (j < 100)
    {
        i = 0;
        while (i < 1000)
        {
            if (i % j == 0)
                buffer[i] = '\n';
            else
                buffer[i] = 'a';
            write(fd, buffer, i);
            i++;
        }
        j++;
    }
    close(fd);
}

int main(void)
{
    int pid;
    int stat_loc;
    int pip[2];

    if (pipe(pip))
    {
        write(2, "PIPE ERROR\n", strlen("PIPE ERROR\n"));
        return (1);
    }
    pid = fork();
    if (pid == 0)
    {
        close(pip[0]);
        writer(pip[1]);
        return (0);
    }
    close(pip[1]);
    reader(pip[0]);
    wait(&stat_loc);
    return (0);
}

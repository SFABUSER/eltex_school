#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
    int fd_fifo;
    char *filename = "/tmp/fifo_file";
    char *buf = "Hi!";

    unlink(filename);

    if (mkfifo(filename, 0666) < 0)
    {
        printf("Не удалось создать канал!\n");
        exit(1);
    }

    fd_fifo = open(filename, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd_fifo < 0)
    {
        printf("Не удалось открыть канал!\n");
        exit(1);
    }

    write(fd_fifo, buf, strlen(buf));
    close(fd_fifo);

    exit(0);
}
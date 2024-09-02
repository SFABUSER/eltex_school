#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    char *filename = "/tmp/fifo_file";
    int fd_fifo;
    char buf[1024];

    fd_fifo = open(filename, O_RDONLY);
    if (fd_fifo < 0)
    {
        printf("Не удалось создать канал!\n");
        exit(1);
    }

    ssize_t bytes_read = read(fd_fifo, buf, 1024);
    if (bytes_read < 0)
    {
        printf("Не удалось открыть канал!\n");
        exit(1);
    }

    buf[bytes_read] = '\0';
    printf("%s\n", buf);
    close(fd_fifo);

    exit(0);
}
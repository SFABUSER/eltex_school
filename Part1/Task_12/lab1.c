#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG "Hi"
#define MSG_SIZE 3

void write_to_file(int *file)
{
    close(file[0]);
    write(file[1], MSG, MSG_SIZE);
    close(file[1]);
}

void read_from_file(int *file)
{
    char buf[MSG_SIZE];

    close(file[1]);
    read(file[0], buf, MSG_SIZE);
    write(STDOUT_FILENO, buf, MSG_SIZE);
    close(file[0]);
}

int main()
{
    pid_t pid;
    int rv;
    int fd[2];

    if (pipe(fd))
    {
        printf("Не удалось создать канал!\n");
        return 1;
    }

    pid = fork();

    switch (pid)
    {
    case -1:
        printf("\nНе удалось создать процесс!\n");
        exit(1);
        break;

    case 0:
        read_from_file(fd);
        exit(rv);
        break;

    default:
        write_to_file(fd);
        printf("\nОжидание завершения процесса %d... \n", pid);
        waitpid(pid, &rv, 0);
        break;
    }
}
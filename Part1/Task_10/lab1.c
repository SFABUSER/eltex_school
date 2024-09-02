#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    pid_t pid;
    int rv;
    pid = fork();

    switch(pid)
    {
        case -1:
            printf("\nНе удалось создать процесс!\n");
            exit(1);
        break;

        case 0:
            printf("Дочерний процесс:\n");
            printf("PID: %d\n", getpid());
            printf("PPID: %d\n", getppid());
            exit(rv);
        break;

        default:
            printf("Родительский процесс:\n");
            printf("PID: %d\n", getpid());
            printf("PPID: %d\n", getppid());
            printf("Ожидание завершения процесса %d... \n", pid);
            waitpid(pid, &rv, 0);
        break;
    }    
}
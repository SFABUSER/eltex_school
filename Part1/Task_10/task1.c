#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;
    pid = fork();

    if (pid < 0) {
        perror("Процесс не создался");
        exit(1);
    } else if (pid == 0) {
        printf("Дочерний процесс:\n");
        printf("PID: %d\n", getpid());
        printf("PPID: %d\n", getppid());
        exit(0);
    } else {
        printf("Родительский процесс:\n");
        printf("PID: %d\n", getpid());
        printf("Дочерний PID: %d\n", pid);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился со статусом: %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс не завершился\n");
        }
    }
    return 0;
}

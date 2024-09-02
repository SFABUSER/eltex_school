#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 100

void execute_command(char *command, char **args) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0) {
        execvp(command, args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
        exit(1);
    }
}

int main() {
    char line[MAX_LINE_LENGTH];
    char *command;
    char *args[MAX_ARGS];

    while (1) {
        printf("> ");
        fgets(line, MAX_LINE_LENGTH, stdin);

        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        command = strtok(line, " \n");
        int i = 0;
        while ((args[i] = strtok(NULL, " \n")) != NULL) {
            i++;
        }
        args[i] = NULL;

        execute_command(command, args);
    }

    return 0;
}
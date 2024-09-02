#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

void execute_pipe(char **args1, char **args2) {
  int pipefd[2];
  pid_t pid1, pid2;

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }

  pid1 = fork();
  if (pid1 == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    execvp(args1[0], args1);
    perror("execvp");
    exit(1);
  }

  pid2 = fork();
  if (pid2 == 0) {
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[1]);
    close(pipefd[0]);
    execvp(args2[0], args2);
    perror("execvp");
    exit(1);
  }

  close(pipefd[0]);
  close(pipefd[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
}

int main() {
  char line[MAX_LINE_LENGTH];
  char *args1[MAX_ARGS];
  char *args2[MAX_ARGS];

  while (1) {
    printf("> ");
    if (!fgets(line, MAX_LINE_LENGTH, stdin)) {
      break;
    }

    if (strcmp(line, "exit\n") == 0) {
      break;
    }

    char *pipe_pos = strchr(line, '|');
    if (pipe_pos) {
      *pipe_pos = '\0';
      pipe_pos++;

      while (*pipe_pos == ' ')
        pipe_pos++;
      char *command1 = strtok(line, " \n");
      int i = 0;
      args1[i++] = command1;
      while ((args1[i] = strtok(NULL, " \n")) != NULL) {
        i++;
      }
      args1[i] = NULL;

      char *command2 = strtok(pipe_pos, " \n");
      i = 0;
      args2[i++] = command2;
      while ((args2[i] = strtok(NULL, " \n")) != NULL) {
        i++;
      }
      args2[i] = NULL;

      execute_pipe(args1, args2);
    } else {
      char *command = strtok(line, " \n");
      int i = 0;
      args1[i++] = command;

      while ((args1[i] = strtok(NULL, " \n")) != NULL) {
        i++;
      }
      args1[i] = NULL;

      execute_command(args1[0], args1);
    }
  }

  return 0;
}
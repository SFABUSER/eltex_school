#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sighandler(int signum)
{
    printf("Received signal %d\n", signum);
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaddset(&sa.sa_mask, SIGUSR1) == -1)
    {
        perror("sigaddset error");
        exit(1);
    }

    sigset_t sigint_mask;
    sigemptyset(&sigint_mask);
    sigaddset(&sigint_mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &sigint_mask, NULL) == -1)
    {
        perror("sigaction error");
        exit(1);
    }

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction error");
        exit(1);
    }

    printf("Waiting for signal...\n");
    while (1)
    {
        sleep(1);
    }
}
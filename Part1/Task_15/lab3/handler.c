#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    sigset_t sigusr1_mask;
    int recieved_signal;

    sigemptyset(&sigusr1_mask);
    sigaddset(&sigusr1_mask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &sigusr1_mask, NULL) == -1)
    {
        perror("sigaction error");
        exit(1);
    }

    printf("Waiting for signal...\n");
    
    while (1)
    {
        if (sigwait(&sigusr1_mask, &recieved_signal) > 0)
        {
            perror("sigwait error");
            exit(1);
        }
        printf("Recieved signal: %d\n", recieved_signal);
    }
}
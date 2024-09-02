#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *printTID()
{
    printf("TID: %ld\n", pthread_self());
}

int main(void)
{
    pthread_t threads[5];
    int status;
    int join_status;

    for (int i = 0; i < 5; i++)
    {
        status = pthread_create(&threads[i], NULL, printTID, NULL);
        if (status != 0)
        {
            printf("Can't create thread, status = %d\n", status);
            exit(0);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        status = pthread_join(threads[i], (void **)&join_status);
    }
}
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;
int counter = 0;

void *printTID()
{
    pthread_mutex_lock(&mutex);

    counter++;
    printf("Counter value: %d\n", counter);

    pthread_mutex_unlock(&mutex);
}

int main(void)
{
    pthread_t threads[5];
    int status;
    int join_status;
    pthread_mutex_init(&mutex, NULL);

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
        status = pthread_join(threads[i], &join_status);
    }

    printf("Final counter value: %d\n", counter);
    pthread_mutex_destroy(&mutex);
}
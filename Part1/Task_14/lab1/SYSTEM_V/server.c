#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>

#define MSG_SIZE 1024
#define KEY 1111

int main()
{
    int shmid;
    char *shmptr;

    if ((shmid = shmget(KEY, MSG_SIZE * sizeof(char), IPC_CREAT | 0666)) == -1)
    {
        perror("shmget server error!\n");
        exit(1);
    }

    if ((shmptr = (char *)shmat(shmid, NULL, 0)) == -1)
    {
        perror("shmat server error!\n");
        exit(1);
    }

    snprintf(shmptr, MSG_SIZE, "Hi!");

    while (strcmp(shmptr, "Hello!"))
    {
        sleep(1);
    }
    printf("%s\n", shmptr);

    if (shmdt(shmptr) == -1)
    {
        perror("shmdt server error!\n");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl server error!\n");
        exit(1);
    }
}
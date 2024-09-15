#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

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

    printf("%s\n", shmptr);
    snprintf(shmptr, MSG_SIZE, "Hello!");
}
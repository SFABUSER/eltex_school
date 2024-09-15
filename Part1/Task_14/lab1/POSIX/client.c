#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/shm_file"
#define MSG_SIZE 1024

int main()
{
    int shm_fd;
    char *shmptr;

    if ((shm_fd = shm_open(SHM_NAME, O_RDWR, 0666)) == -1)
    {
        perror("shm_open client error!\n");
        exit(1);
    }

    if ((shmptr = (char *)mmap(0, MSG_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == -1)
    {
        perror("mmap client error!\n");
        exit(1);
    }

    printf("%s\n", shmptr);
    snprintf(shmptr, MSG_SIZE, "Hello!");

    if (munmap(shmptr, MSG_SIZE) == -1)
    {
        perror("munmap client error!\n");
        exit(1);
    }
}
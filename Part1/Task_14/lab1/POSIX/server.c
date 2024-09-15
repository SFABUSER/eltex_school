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

    if ((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("shm_open server error!\n");
        exit(1);
    }

    if (ftruncate(shm_fd, MSG_SIZE) == -1)
    {
        perror("ftruncate server error!\n");
        exit(1);
    }

    if ((shmptr = (char *)mmap(0, MSG_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == -1)
    {
        perror("mmap server error!\n");
        exit(1);
    }

    snprintf(shmptr, MSG_SIZE, "Hi!");

    while (strcmp(shmptr, "Hello!"))
    {
        sleep(1);
    }
    printf("%s\n", shmptr);

    if (munmap(shmptr, MSG_SIZE) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }

    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }
}
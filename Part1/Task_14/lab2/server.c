#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SHM_CHAT "/shm_chat"
#define SHM_SEM "/shm_sem"
#define MAX_MESSAGES_LENGTH 100
#define MAX_MSG_LENGTH 512
#define MAX_NAME_LENGTH 64

typedef struct
{
    char abonent_name[MAX_MESSAGES_LENGTH];
    char data[MAX_MSG_LENGTH];
} message;

typedef struct
{
    char abonent_name[MAX_NAME_LENGTH];
    int type;
} new_connection;

typedef struct
{
    int connections_length;
    int messages_length;
    new_connection connections[MAX_MESSAGES_LENGTH];
    message messages[MAX_MESSAGES_LENGTH];
} shm_chat;

shm_chat *shm_ptr;
sem_t *sem;

int main()
{
    int shm_fd;

    if ((shm_fd = shm_open(SHM_CHAT, O_CREAT | O_RDWR, 0666)) == -1)
    {
        perror("shm_open server error!\n");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(shm_chat)) == -1)
    {
        perror("ftruncate server error!\n");
        exit(1);
    }

    if ((shm_ptr = (shm_chat *)mmap(0, sizeof(shm_chat), PROT_WRITE, MAP_SHARED, shm_fd, 0)) == -1)
    {
        perror("mmap server error!\n");
        exit(1);
    }

    sem = sem_open(SHM_SEM, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_wait(sem);

    shm_ptr->connections_length = 0;
    shm_ptr->messages_length = 0;

    sem_post(sem);

    printf("Server is working! Enter '/q' for shutdown\n");
    while (1)
    {
        char buf[MAX_MSG_LENGTH];
        fgets(buf, MAX_MSG_LENGTH, stdin);
        buf[strcspn(buf, "\n")] = '\0';

        if (strcmp(buf, "/q") == 0) break;
    }

    if (munmap(shm_ptr, sizeof(shm_chat)) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }

    if (shm_unlink(SHM_CHAT) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }

    if (sem_close(sem) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }

    if (sem_unlink(SHM_SEM) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }
}
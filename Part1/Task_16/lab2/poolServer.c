#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MSG_SIZE 128
#define PORT 7777
#define POOL_SIZE 5

struct sockaddr_in server, client;
pthread_mutex_t mutexes[POOL_SIZE];
pthread_cond_t conds[POOL_SIZE];
int connect_fd_arr[POOL_SIZE];
int fd_ready_arr[POOL_SIZE];

void init_params()
{
    for (int i = 0; i < POOL_SIZE; i++)
    {
        pthread_mutex_init(&mutexes[i], NULL);
        pthread_cond_init(&conds[i], NULL);
        fd_ready_arr[i] = 0;
    }
}

void *new_connection(void *arg)
{
    int id = *(int *)arg;
    char message[MSG_SIZE];
    int bytes_received;

    while (1)
    {
        pthread_mutex_lock(&mutexes[id]);
        printf("%d locked\n", id);

        while (!fd_ready_arr[id])
        {
            printf("%d wait for signal\n", id);
            pthread_cond_wait(&conds[id], &mutexes[id]);
        }

        printf("%d work\n", id);
        while ((bytes_received = recv(connect_fd_arr[id], message, MSG_SIZE, 0)) > 0)
        {
            printf("%s\n", message);
            send(connect_fd_arr[id], message, MSG_SIZE, 0);
        }

        close(connect_fd_arr[id]);
        fd_ready_arr[id] = 0;
        memset(message, 0, MSG_SIZE);
        pthread_mutex_unlock(&mutexes[id]);
        printf("%d unlocked\n", id);
    }
}

void main()
{
    init_params();
    pthread_t threads[POOL_SIZE];
    int thread_id_arr[POOL_SIZE];

    for (int i = 0; i < POOL_SIZE; i++)
    {
        thread_id_arr[i] = i;
    }

    for (int i = 0; i < POOL_SIZE; i++)
    {
        if (pthread_create(&threads[i], NULL, new_connection, &thread_id_arr[i]) != 0)
        {
            perror("Can't create thread\n");
            exit(0);
        }
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    socklen_t client_size = sizeof(client);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));
    listen(sock_fd, POOL_SIZE);

    while (1)
    {
        for (int i = 0; i < POOL_SIZE; i++)
        {
            if (pthread_mutex_trylock(&mutexes[i]) == 0)
            {
                if (!fd_ready_arr[i])
                {
                    pthread_mutex_unlock(&mutexes[i]);
                    connect_fd_arr[i] = accept(sock_fd, (struct sockaddr *)&client, &client_size);
                    pthread_mutex_lock(&mutexes[i]);
                    fd_ready_arr[i] = 1;
                    pthread_cond_signal(&conds[i]);
                    pthread_mutex_unlock(&mutexes[i]);
                }
                else
                {
                    pthread_mutex_unlock(&mutexes[i]);
                }
            }
        }
        sleep(1);
    }

    close(sock_fd);
}
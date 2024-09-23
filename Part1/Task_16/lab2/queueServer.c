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
#define QUEUE_SIZE 5

struct sockaddr_in server, client;

typedef struct
{
    int clients_fd[QUEUE_SIZE];
    int current_client;
    int write_pos;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue;

queue q;

void init_queue(queue new_queue)
{
    new_queue.current_client = 0;
    new_queue.write_pos = 0;
    new_queue.size = 0;
    pthread_mutex_init(&new_queue.mutex, NULL);
    pthread_cond_init(&new_queue.not_empty, NULL);
    pthread_cond_init(&new_queue.not_full, NULL);
}

void add_client(int client_fd)
{
    pthread_mutex_lock(&q.mutex);

    while (q.size == QUEUE_SIZE)
    {
        pthread_cond_wait(&q.not_full, &q.mutex);
    }

    q.clients_fd[q.write_pos] = client_fd;
    q.size++;
    q.write_pos = (q.write_pos + 1) % QUEUE_SIZE;

    pthread_cond_signal(&q.not_empty);
    pthread_mutex_unlock(&q.mutex);
}

int get_client()
{
    pthread_mutex_lock(&q.mutex);

    while (!q.size)
    {
        pthread_cond_wait(&q.not_empty, &q.mutex);
    }

    int client_fd = q.clients_fd[q.current_client];
    q.current_client = (q.current_client + 1) % QUEUE_SIZE;
    q.size--;

    pthread_cond_signal(&q.not_full);
    pthread_mutex_unlock(&q.mutex);

    return client_fd;
}

void *new_connection(void *arg)
{
    int id = *(int *)arg;
    char message[MSG_SIZE];
    int bytes_received;
    int client_fd;

    while (1)
    {
        client_fd = get_client();

        while ((bytes_received = recv(client_fd, message, MSG_SIZE, 0)) > 0)
        {
            printf("%s\n", message);
            send(client_fd, message, MSG_SIZE, 0);
        }
        
        close(client_fd);
        memset(message, 0, MSG_SIZE);
    }
}

main()
{
    pthread_t threads[QUEUE_SIZE];
    int thread_id_arr[QUEUE_SIZE];

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        thread_id_arr[i] = i;
    }

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        if (pthread_create(&threads[i], NULL, new_connection, &thread_id_arr[i]) != 0)
        {
            perror("Can't create thread\n");
            exit(0);
        }
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    int connect_fd;

    socklen_t client_size = sizeof(client);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = INADDR_ANY;

    init_queue(q);

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));
    listen(sock_fd, QUEUE_SIZE);

    while (1)
    {
        connect_fd = accept(sock_fd, (struct sockaddr *)&client, &client_size);
        add_client(connect_fd);
        sleep(1);
    }

    close(sock_fd);
}
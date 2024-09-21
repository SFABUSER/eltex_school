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

struct sockaddr_in server, client;

void *new_connection(void *arg)
{
    int client_sock = *(int *)arg;
    char message[MSG_SIZE];
    int bytes_received;

    recv(client_sock, message, MSG_SIZE, 0);
    printf("%s\n", message);
    send(client_sock, message, MSG_SIZE, 0);

    close(client_sock);
    return NULL;
}

void main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    int connect_fd;
    pthread_t thread;
    socklen_t client_size = sizeof(client);
    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));
    
    listen(sock_fd, 5);

    while (1)
    {
        connect_fd = accept(sock_fd, (struct sockaddr *)&client, &client_size);

        if (pthread_create(&thread, NULL, new_connection, &connect_fd) != 0)
        {
            printf("Can't create thread\n");
            exit(1);
        }
    }

    close(sock_fd);
}
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_FILENAME "/server_queue"
#define CLIENT_FILENAME "/client_queue"
#define MSG_SIZE 1024

int main()
{
    mqd_t server_queue, client_queue;
    char buf[MSG_SIZE];

    if ((server_queue = mq_open(SERVER_FILENAME, O_RDONLY)) == -1)
    {
        perror("mq_open server: ");
        exit(1);
    }

    if (mq_receive(server_queue, buf, MSG_SIZE, NULL) == -1)
    {
        perror("mq_recieve server: ");
        exit(1);
    }
    printf("Received message: %s\n", buf);

    if (mq_close(server_queue) == -1)
    {
        perror("mq_close server: ");
        exit(1);
    }

    if ((client_queue = mq_open(CLIENT_FILENAME, O_WRONLY)) == -1)
    {
        perror("mq_open client: ");
        exit(1);
    }

    snprintf(buf, MSG_SIZE, "Hello!");
    if (mq_send(client_queue, buf, MSG_SIZE, 0) == -1)
    {
        perror("mq_send client: ");
        exit(1);
    }
    printf("Send message: %s\n", buf);

    if (mq_close(client_queue) == -1)
    {
        perror("mq_close client: ");
        exit(1);
    }
}
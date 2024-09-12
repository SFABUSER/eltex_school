#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_SIZE 1024
#define SERVER_FILENAME "/server_queue"
#define CLIENT_FILENAME "/client_queue"

int main()
{
    mqd_t server_queue, client_queue;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = MSG_SIZE;

    char buf[MSG_SIZE];

    if ((server_queue = mq_open(SERVER_FILENAME, O_CREAT | O_WRONLY, 0644, &attr)) == -1)
    {
        perror("mq_open server: ");
        exit(1);
    }

    snprintf(buf, MSG_SIZE, "Hi!");

    if (mq_send(server_queue, buf, MSG_SIZE, 0) == -1)
    {
        perror("mq_send server: ");
        exit(1);
    }

    printf("Send message: %s\n", buf);

    if (mq_close(server_queue) == -1)
    {
        perror("mq_close server: ");
        exit(1);
    }

    if ((client_queue = mq_open(CLIENT_FILENAME, O_CREAT | O_RDONLY, 0644, &attr)) == -1)
    {
        perror("mq_open client: ");
        exit(1);
    }

    if (mq_receive(client_queue, buf, MSG_SIZE, NULL) == -1)
    {
        perror("mq_receive client: ");
        exit(1);
    }

    printf("Received message: %s\n", buf);

    if (mq_close(client_queue) == -1)
    {
        perror("mq_close server: ");
        exit(1);
    }

    if (mq_unlink(SERVER_FILENAME) == -1)
    {
        perror("mq_unlink server: ");
        exit(1);
    }

    if (mq_unlink(CLIENT_FILENAME) == -1)
    {
        perror("mq_unlink client: ");
        exit(1);
    }
}
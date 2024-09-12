#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENT_SIZE 10
#define MAX_MSG_SIZE 256
#define MAX_NAME_SIZE 32
#define CHAT_QUEUE_NAME "/chat_queue"

typedef struct
{
    char text[MAX_MSG_SIZE];
    char name[MAX_NAME_SIZE];
} Message;

mqd_t mq;
int client_count = 0;
char client_name[MAX_CLIENT_SIZE][MAX_NAME_SIZE];

void sigint_handler(int sig)
{
    mq_close(mq);
    mq_unlink(CHAT_QUEUE_NAME);
    printf("\nServer shutting down\n");
    exit(0);
}

void broadcast_message(Message *msg)
{
    for (int i = 0; i < client_count; i++)
    {
        mqd_t client_mq = mq_open(client_name[i], O_WRONLY);
        if (client_mq == ((mqd_t)-1))
        {
            perror("Server: mq_open failed");
            continue;
        }
        if (mq_send(client_mq, (const char *)msg, sizeof(Message), 0) == -1)
        {
            perror("Server: mq_send failed");
        }
        mq_close(client_mq);
    }
}

void send_user_list_to_client(const char *client_queue_name)
{
    mqd_t client_mq = mq_open(client_queue_name, O_WRONLY);
    if (client_mq == ((mqd_t)-1))
    {
        perror("Server: mq_open failed");
        return;
    }
    for (int i = 0; i < client_count; i++)
    {
        Message user_msg;
        strncpy(user_msg.name, client_name[i], MAX_NAME_SIZE);
        strncpy(user_msg.text, "/join", MAX_MSG_SIZE);
        if (mq_send(client_mq, (const char *)&user_msg, sizeof(Message), 0) == -1)
        {
            perror("Server: mq_send failed");
        }
    }
    mq_close(client_mq);
}

int main()
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;

    mq = mq_open(CHAT_QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == ((mqd_t)-1))
    {
        perror("mq_open failed");
        exit(1);
    }

    signal(SIGINT, sigint_handler);
    printf("Server started\n");

    while (1)
    {
        Message msg;
        if (mq_receive(mq, (char *)&msg, sizeof(Message), NULL) == -1)
        {
            perror("mq_receive failed");
            continue;
        }

        if (strncmp(msg.text, "/join", 5) == 0)
        {
            if (client_count < MAX_CLIENT_SIZE)
            {
                snprintf(client_name[client_count], MAX_NAME_SIZE, "/client_%s",
                         msg.name);
                client_count++;

                broadcast_message(&msg);
                send_user_list_to_client(client_name[client_count - 1]);

                snprintf(msg.text, MAX_MSG_SIZE, "%s joined the chat", msg.name);
                printf("%s\n", msg.text);
            }
            else
            {
                snprintf(msg.text, MAX_MSG_SIZE, "Chat is full");
                printf("%s\n", msg.text);
            }
        }
        else
        {
            printf("%s: %s\n", msg.name, msg.text);
            broadcast_message(&msg);
        }
    }

    sigint_handler(0);
    return 0;
}
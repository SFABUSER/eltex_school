#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MSG_SIZE 1024
#define SERVER_KEY 1111
#define CLIENT_KEY 2222

typedef struct
{
    long type;
    char data[MSG_SIZE];
} message;

int main()
{
    int server_queue_id, client_queue_id;
    message msg;

    if ((server_queue_id = msgget(SERVER_KEY, IPC_CREAT | 0666)) == -1)
    {
        printf("msgget server error!\n");
        exit(1);
    }

    msg.type = 1;
    snprintf(msg.data, MSG_SIZE, "Hi!");

    if (msgsnd(server_queue_id, &msg, sizeof(msg.data), 0) == -1)
    {
        printf("msgsnd server error!\n");
        exit(1);
    }

    printf("Send message: %s\n", msg.data);

    if ((client_queue_id = msgget(CLIENT_KEY, IPC_CREAT | 0666)) == -1)
    {
        printf("msgget client error!\n");
        exit(1);
    }

    if (msgrcv(client_queue_id, &msg, sizeof(msg.data), 1, 0) == -1)
    {
        printf("msgsnd client error!\n");
        exit(1);
    }

    printf("Recieved message: %s\n", msg.data);

    if (msgctl(server_queue_id, IPC_RMID, NULL) == -1)
    {
        printf("msgctl server error!\n");
        exit(1);
    }

    if (msgctl(client_queue_id, IPC_RMID, NULL) == -1)
    {
        printf("msgctl client error!\n");
        exit(1);
    }
}
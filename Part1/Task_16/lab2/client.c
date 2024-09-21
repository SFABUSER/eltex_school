#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_SIZE 128
#define PORT 7777

struct sockaddr_in server;

void main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    char message[MSG_SIZE];

    server.sin_family = AF_INET;
    server.sin_port = PORT;
    server.sin_addr.s_addr = INADDR_ANY;

    connect(sock_fd, (struct sockaddr *)&server, sizeof(server));
    send(sock_fd, "Hello!", 7, 0);
    recv(sock_fd, message, MSG_SIZE, 0);
    printf("%s\n", message);
} 
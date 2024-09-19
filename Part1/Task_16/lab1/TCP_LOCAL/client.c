#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>

#define MSG_SIZE 128

struct sockaddr_un server;

void main()
{
    int sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    char message[MSG_SIZE];

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "server_path");

    connect(sock_fd, (struct sockaddr*)&server, sizeof(server));
    write(sock_fd, "Hello!", 7);
    read(sock_fd, message, MSG_SIZE);
    printf("%s\n", message);
}
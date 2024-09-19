#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>

#define MSG_SIZE 128

struct sockaddr_un server, client;

void main()
{
    int sock_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    char message[MSG_SIZE];

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "server_path");

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));

    socklen_t size = sizeof(client);
    recvfrom(sock_fd, message, MSG_SIZE, 0, (struct sockaddr *)&client, &size);
    sendto(sock_fd, "Hi!", 4, 0, (struct sockaddr *)&client, sizeof(client));

    close(sock_fd);
}
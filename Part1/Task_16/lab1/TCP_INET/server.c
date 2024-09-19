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
    int connect_fd;
    char message[MSG_SIZE];

    server.sin_family = AF_INET;
    server.sin_port = PORT;
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));
    listen(sock_fd, 5);

    connect_fd = accept(sock_fd, NULL, NULL);

    recv(connect_fd, message, MSG_SIZE, 0);
    printf("%s\n", message);
    send(connect_fd, "Hi!", 4, 0);

    close(sock_fd);
    close(connect_fd);
}
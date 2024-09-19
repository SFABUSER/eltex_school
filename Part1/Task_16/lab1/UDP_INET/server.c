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

struct sockaddr_in server, client;

void main()
{
    char message[MSG_SIZE];

    server.sin_family = AF_INET;
    server.sin_port = PORT;
    inet_pton(AF_INET, "127.0.0.5", &server.sin_addr);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));

    socklen_t size = sizeof(client);
    recvfrom(sock_fd, message, MSG_SIZE, 0, (struct sockaddr *)&client, &size);
    printf("%s\n", message);
    sendto(sock_fd, "Hi!", 4, 0, (struct sockaddr *)&client, sizeof(client));

    close(sock_fd);
}
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

struct sockaddr_in client;

void main()
{
    char message[MSG_SIZE];

    client.sin_family = AF_INET;
    client.sin_port = PORT;
    inet_pton(AF_INET, "255.255.255.255", &client.sin_addr);
    socklen_t client_size = sizeof(client);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sock_fd, (struct sockaddr *)&client, sizeof(client));

    recvfrom(sock_fd, message, MSG_SIZE, 0, (struct sockaddr *)&client, &client_size);
    printf("%s\n", message);
}
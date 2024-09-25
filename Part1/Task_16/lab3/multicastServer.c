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
    int flag = 1;

    client.sin_family = AF_INET;
    client.sin_port = PORT;
    inet_pton(AF_INET, "224.0.0.1", &client.sin_addr);
    socklen_t client_size = sizeof(client);
    
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

    sendto(sock_fd, "Multicast message!", MSG_SIZE, 0, (struct sockaddr *)&client, client_size);

    close(sock_fd);
}
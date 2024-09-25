#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_SIZE 128
#define PORT 7777

struct sockaddr_in client;
struct ip_mreqn group;

void main()
{
    char message[MSG_SIZE];

    client.sin_family = AF_INET;
    client.sin_port = PORT;
    client.sin_addr.s_addr = INADDR_ANY;
    socklen_t client_size = sizeof(client);

    inet_pton(AF_INET, "224.0.0.1", &group.imr_multiaddr.s_addr);
    group.imr_address.s_addr = INADDR_ANY;
    group.imr_ifindex = 0;

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sock_fd, (struct sockaddr*)&client, sizeof(client));

    setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));

    recvfrom(sock_fd, message, MSG_SIZE, 0, (struct sockaddr *)&client, &client_size);
    printf("%s\n", message);

    setsockopt(sock_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &group, sizeof(group));
}
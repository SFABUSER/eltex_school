#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

#define SERVER_PORT 7777
#define CLIENT_PORT 7778
#define MSG_SIZE 128

struct sockaddr_in server, client;
struct udphdr udp_header;

void main()
{
    char message[MSG_SIZE] = "Hello!";
    char buffer[sizeof(struct udphdr) + strlen(message)];

    server.sin_family = AF_INET;
    server.sin_port = SERVER_PORT;
    inet_pton(AF_INET, "127.0.0.5", &server.sin_addr);

    udp_header.source = CLIENT_PORT;
    udp_header.dest = SERVER_PORT;
    udp_header.len = htons(sizeof(struct udphdr) + strlen(message));
    udp_header.check = 0;

    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    memcpy(buffer, &udp_header, sizeof(struct udphdr));
    memcpy(buffer + sizeof(struct udphdr), message, strlen(message));

    sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server, sizeof(server));

    close(sock_fd);
}
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
#include <netinet/ip.h>

#define SERVER_PORT 7777
#define CLIENT_PORT 7778
#define MSG_SIZE 128

struct sockaddr_in server, client;
struct udphdr udp_header;
struct iphdr ip_header;

void main()
{
    char message[MSG_SIZE] = "Hello!";
    char buffer[sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message)];

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.5", &server.sin_addr);

    ip_header.version = 4;
    ip_header.ihl = 5;
    ip_header.tos = 0;
    ip_header.tot_len = htons(sizeof(struct udphdr) + sizeof(struct iphdr) + strlen(message));
    ip_header.id = htons(54321);
    ip_header.frag_off = 0;
    ip_header.ttl = 64;
    ip_header.protocol = IPPROTO_UDP;
    ip_header.check = 0;
    ip_header.saddr = inet_addr("127.0.0.2");
    ip_header.daddr = server.sin_addr.s_addr;
    
    udp_header.source = htons(CLIENT_PORT);
    udp_header.dest = htons(SERVER_PORT);
    udp_header.len = htons(sizeof(struct udphdr) + strlen(message));
    udp_header.check = 0;
    
    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    
    int opt = 1;
    setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));
    
    memcpy(buffer, &ip_header, sizeof(struct iphdr));
    memcpy(buffer + sizeof(struct iphdr), &udp_header, sizeof(struct udphdr));
    memcpy(buffer + sizeof(struct iphdr) + sizeof(struct udphdr), message, strlen(message));
    
    sendto(sock_fd, buffer, sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message), 0, (struct sockaddr *)&server, sizeof(server));

    close(sock_fd);
}
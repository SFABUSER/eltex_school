#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

#define SERVER_PORT 7777
#define CLIENT_PORT 7778
#define MSG_SIZE 128

void parse_mac_address(const char *mac_str, unsigned char *mac) {
    sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

int main()
{
    char message[MSG_SIZE] = "Hello!";
    char buffer[sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message)];

    struct sockaddr_ll device;
    struct ethhdr *eth_header = (struct ethhdr *)buffer;
    struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));

    unsigned char dest_mac[ETH_ALEN];
    unsigned char src_mac[ETH_ALEN];

    parse_mac_address("02:42:49:c1:c2:ce", dest_mac);
    parse_mac_address("c0:e4:34:dc:ce:03", src_mac);

    memcpy(eth_header->h_dest, dest_mac, ETH_ALEN);
    memcpy(eth_header->h_source, src_mac, ETH_ALEN);
    eth_header->h_proto = htons(ETH_P_IP);

    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message));
    ip_header->id = htons(54321);
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->check = 0;
    ip_header->saddr = inet_addr("127.0.0.2");
    ip_header->daddr = inet_addr("127.0.0.5");

    udp_header->source = htons(CLIENT_PORT);
    udp_header->dest = htons(SERVER_PORT);
    udp_header->len = htons(sizeof(struct udphdr) + strlen(message));
    udp_header->check = 0;

    memcpy(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr), message, strlen(message));

    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    memset(&device, 0, sizeof(device));
    device.sll_family = AF_PACKET;
    device.sll_protocol = htons(ETH_P_IP);
    device.sll_ifindex = if_nametoindex("wlp4s0");
    device.sll_halen = ETH_ALEN;
    memcpy(device.sll_addr, eth_header->h_dest, ETH_ALEN);

    if (sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&device, sizeof(device)) == -1)

    close(sock_fd);
    return 0;
}
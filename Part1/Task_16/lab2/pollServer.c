#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <poll.h>

#define MSG_SIZE 128
#define TCP_PORT 7777
#define UDP_PORT 7778
#define MAX_CLIENTS 10

struct sockaddr_in tcp_server, tcp_client, udp_server, udp_client;

void init_server(struct sockaddr_in *s, int port)
{
    s->sin_family = AF_INET;
    s->sin_port = port;
    s->sin_addr.s_addr = INADDR_ANY;
}

void udp_client_handler(int fd)
{
    char message[MSG_SIZE];
    socklen_t udp_client_size = sizeof(udp_client);

    recvfrom(fd, message, MSG_SIZE, 0, (struct sockaddr *)&udp_client, &udp_client_size);
    printf("UDP: %s\n", message);
    sendto(fd, "Hi!", 4, 0, (struct sockaddr *)&udp_client, udp_client_size);
}

void tcp_client_handler(int fd)
{
    char message[MSG_SIZE];
    int bytes_received;

    while ((bytes_received = recv(fd, message, MSG_SIZE, 0)) > 0)
    {
        printf("TCP: %s\n", message);
        send(fd, message, MSG_SIZE, 0);
    }

    close(fd);
}

void main()
{
    int tcp_sock_fd, udp_sock_fd;
    struct pollfd poll_fds[MAX_CLIENTS + 2];
    int fd_exist[MAX_CLIENTS + 2];

    for (int i = 0; i < MAX_CLIENTS + 2; i++)
    {
        fd_exist[i] = 0;
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN;
    }

    init_server(&tcp_server, TCP_PORT);
    init_server(&udp_server, UDP_PORT);

    socklen_t tcp_client_size = sizeof(tcp_client);

    tcp_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(tcp_sock_fd, (struct sockaddr *)&tcp_server, sizeof(tcp_server));
    listen(tcp_sock_fd, 5);

    udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(udp_sock_fd, (struct sockaddr *)&udp_server, sizeof(udp_server));

    poll_fds[0].fd = tcp_sock_fd;
    fd_exist[0] = 1;
    poll_fds[1].fd = udp_sock_fd;
    fd_exist[1] = 1;

    while (1)
    {
        printf("dfhgdh\n");
        poll(poll_fds, MAX_CLIENTS + 2, -1);

        for (int i = 0; i < MAX_CLIENTS + 2; i++)
        {
            if (fd_exist[i])
            {   
                if (poll_fds[i].revents & POLLIN)
                {
                    if (poll_fds[i].fd == tcp_sock_fd)
                    {
                        int new_client = accept(tcp_sock_fd, (struct sockaddr *)&tcp_client, &tcp_client_size);

                        for (int j = 2; j < MAX_CLIENTS + 2; j++)
                        {
                            if (!fd_exist[j])
                            {
                                poll_fds[j].fd = new_client;
                                poll_fds[j].events = POLLIN;
                                fd_exist[j] = 1;
                                break;
                            }
                        }
                    }
                    else if (poll_fds[i].fd == udp_sock_fd)
                    {
                        udp_client_handler(udp_sock_fd);
                    }
                    else
                    {
                        tcp_client_handler(poll_fds[i].fd);
                        fd_exist[i] = 0;
                        poll_fds[i].fd = -1;
                    }
                }
                poll_fds[i].revents = 0;
            }
        }
    }
}
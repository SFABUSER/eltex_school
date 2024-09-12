#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENT_SIZE 10
#define MAX_MSG_SIZE 256
#define MAX_NAME_SIZE 32
#define CHAT_QUEUE_NAME "/chat_queue"

typedef struct
{
    char text[MAX_MSG_SIZE];
    char name[MAX_NAME_SIZE];
} Message;

mqd_t mq;
mqd_t client_mq;
char client_queue_name[MAX_NAME_SIZE];

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

WINDOW *message_win;
WINDOW *input_win;
WINDOW *info_win;

void sigint_handler(int sig)
{
    mq_close(mq);
    mq_unlink(client_queue_name);
    endwin();
    printf("\nClient shutting down\n");
    exit(0);
}

void *receive_messages(void *arg)
{
    Message msg;
    while (1)
    {
        ssize_t bytes_read =
            mq_receive(client_mq, (char *)&msg, sizeof(Message), NULL);
        if (bytes_read == -1)
        {
            perror("Client: mq_receive failed");
            continue;
        }

        pthread_mutex_lock(&print_mutex);

        if (strcmp(msg.text, "/join") == 0 || strcmp(msg.text, "/quit") == 0)
        {
            if (strcmp(msg.text, "/join") == 0)
            {
                mvwprintw(info_win, getmaxy(info_win) - 2, 1, "%s joined the chat\n",
                          msg.name);
            }
            else
            {
                mvwprintw(info_win, getmaxy(info_win) - 2, 1, "%s left the chat\n",
                          msg.name);
            }
            wrefresh(info_win);
        }
        else
        {
            if (strcmp(msg.name, (char *)arg) != 0)
            {
                wprintw(message_win, "%s: %s\n", msg.name, msg.text);
                wrefresh(message_win);
            }
        }

        pthread_mutex_unlock(&print_mutex);
    }
    return NULL;
}

void init_windows()
{
    int height, width;
    getmaxyx(stdscr, height, width);
    message_win = newwin(height - 3, width - 30, 0, 0);
    scrollok(message_win, TRUE);
    info_win = newwin(height - 3, 30, 0, width - 30);
    input_win = newwin(3, width - 30, height - 3, 0);
    wrefresh(message_win);
    wrefresh(input_win);
    wrefresh(info_win);
}

int main()
{
    char name[MAX_NAME_SIZE];
    Message msg;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_windows();
    mvwprintw(input_win, 1, 1, "Enter your name: ");
    wrefresh(input_win);
    wgetnstr(input_win, name, MAX_NAME_SIZE);
    name[strcspn(name, "\n")] = '\0';
    snprintf(client_queue_name, MAX_NAME_SIZE, "/client_%s", name);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;

    client_mq = mq_open(client_queue_name, O_RDWR | O_CREAT, 0666, &attr);
    if (client_mq == ((mqd_t)-1))
    {
        perror("Client: mq_open failed");
        sigint_handler(0);
    }

    mq = mq_open(CHAT_QUEUE_NAME, O_WRONLY);
    if (mq == ((mqd_t)-1))
    {
        perror("Client: mq_open failed");
        sigint_handler(0);
    }

    strncpy(msg.name, name, MAX_NAME_SIZE);
    strncpy(msg.text, "/join", MAX_MSG_SIZE);
    if (mq_send(mq, (const char *)&msg, sizeof(Message), 0) == -1)
    {
        perror("Client: mq_send failed");
    }
    else
    {
        mvwprintw(info_win, getmaxy(info_win) - 2, 1, "%s joined the chat\n", name);
        wrefresh(info_win);
    }

    signal(SIGINT, sigint_handler);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, name);

    while (1)
    {
        char input[MAX_MSG_SIZE];
        wclear(input_win);
        mvwprintw(input_win, 1, 1, "Message: ");
        wrefresh(input_win);
        wgetnstr(input_win, input, MAX_MSG_SIZE);

        if (strncmp(input, "/quit", 5) == 0)
        {
            sigint_handler(0);
        }
        else
        {
            strncpy(msg.name, name, MAX_NAME_SIZE);
            strncpy(msg.text, input, MAX_MSG_SIZE);
            if (mq_send(mq, (const char *)&msg, sizeof(Message), 0) == -1)
            {
                perror("Client: mq_send failed");
            }
            else
            {
                pthread_mutex_lock(&print_mutex);
                wprintw(message_win, "%s: %s\n", name, input);
                wrefresh(message_win);
                pthread_mutex_unlock(&print_mutex);
            }
        }
    }

    sigint_handler(0);
    return 0;
}
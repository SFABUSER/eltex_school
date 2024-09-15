#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <ncurses.h>

#define SHM_CHAT "/shm_chat"
#define SHM_SEM "/shm_sem"
#define MAX_MESSAGES_LENGTH 100
#define MAX_MSG_LENGTH 512
#define MAX_NAME_LENGTH 64
#define CONNECT 1
#define DISCONNECT 0

typedef struct
{
    char abonent_name[MAX_MESSAGES_LENGTH];
    char data[MAX_MSG_LENGTH];
} message;

typedef struct
{
    char abonent_name[MAX_NAME_LENGTH];
    int type;
} new_connection;

typedef struct
{
    int connections_length;
    int messages_length;
    new_connection connections[MAX_MESSAGES_LENGTH];
    message messages[MAX_MESSAGES_LENGTH];
} shm_chat;

shm_chat *shm_ptr;
sem_t *sem;

char name[MAX_NAME_LENGTH];
int old_connections_length;
int old_messages_length;
int connection_status;

WINDOW *chat_win, *input_win;

void *handle_messages()
{
    while (1)
    {
        sem_wait(sem);

        if (!connection_status)
        {
            sem_post(sem);
            return;
        }

        if (old_connections_length < shm_ptr->connections_length)
        {
            for (int i = old_connections_length; i < shm_ptr->connections_length; i++)
            {
                int row = 1;
                if (shm_ptr->connections[i].type == 1)
                {
                    mvwprintw(chat_win, row, 50, "%s joined the chat!\n", shm_ptr->connections[i].abonent_name);
                }
                else
                {
                    mvwprintw(chat_win, row, 50, "%s leave the chat!\n", shm_ptr->connections[i].abonent_name);
                }
                row++;
            }
            wrefresh(chat_win);
            old_connections_length = shm_ptr->connections_length;
        }

        if (old_messages_length < shm_ptr->messages_length)
        {
            wclear(chat_win);
            wmove(chat_win, 0, 0);
            for (int i = 0; i < shm_ptr->messages_length; i++)
            {
                wprintw(chat_win, "%s: %s\n", shm_ptr->messages[i].abonent_name, shm_ptr->messages[i].data);
            }
            wrefresh(chat_win);
            old_messages_length = shm_ptr->messages_length;
        }

        sem_post(sem);
    }
}

void connections_control(int type)
{
    new_connection connection;
    strncpy(connection.abonent_name, name, MAX_NAME_LENGTH);
    connection.type = type;

    sem_wait(sem);

    connection_status = type;
    old_connections_length = shm_ptr->connections_length;
    old_messages_length = 0;

    shm_ptr->connections[shm_ptr->connections_length] = connection;
    shm_ptr->connections_length++;

    sem_post(sem);
}

int main()
{
    int shm_fd;
    pthread_t thread;
    int status;
    int join_status;

    if ((shm_fd = shm_open(SHM_CHAT, O_RDWR, 0666)) == -1)
    {
        perror("shm_open client error!\n");
        exit(1);
    }

    if ((shm_ptr = (shm_chat *)mmap(0, sizeof(shm_chat), PROT_WRITE, MAP_SHARED, shm_fd, 0)) == -1)
    {
        perror("mmap client error!\n");
        exit(1);
    }

    sem = sem_open(SHM_SEM, 0);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);
    chat_win = newwin(height - 3, width - 30, 0, 0);
    scrollok(chat_win, TRUE);
    input_win = newwin(3, width - 30, height - 3, 0);
    wrefresh(chat_win);
    wrefresh(input_win);

    mvprintw(0, 0, "Enter your name -> ");
    refresh();
    echo();
    mvscanw(1, 0, "%s", name);
    noecho();
    clear();

    connections_control(CONNECT);

    status = pthread_create(&thread, NULL, handle_messages, NULL);
    if (status != 0)
    {
        printf("Can't create thread, status = %d\n", status);
        exit(1);
    }

    while (1)
    {
        char msg_buf[MAX_MSG_LENGTH];
        wclear(input_win);
        mvwprintw(input_win, 1, 1, "Enter message: ");
        wrefresh(input_win);
        echo();
        wgetnstr(input_win, msg_buf, MAX_MSG_LENGTH);
        noecho();

        if (strcmp(msg_buf, "/q") == 0)
        {
            connections_control(DISCONNECT);
            break;
        }

        message new_msg;
        strncpy(new_msg.abonent_name, name, MAX_NAME_LENGTH);
        strncpy(new_msg.data, msg_buf, MAX_MSG_LENGTH);

        sem_wait(sem);

        shm_ptr->messages[shm_ptr->messages_length] = new_msg;
        shm_ptr->messages_length++;

        sem_post(sem);
    }

    status = pthread_join(thread, (void **)&join_status);

    if (munmap(shm_ptr, sizeof(shm_chat)) == -1)
    {
        perror("munmap client error!\n");
        exit(1);
    }

    if (sem_close(sem) == -1)
    {
        perror("munmap server error!\n");
        exit(1);
    }

    delwin(chat_win);
    delwin(input_win);
    endwin();

    return 0;
}
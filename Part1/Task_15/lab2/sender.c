#include <stdio.h>
#include <stdlib.h>

int main()
{
    int option = 0;

    do
    {
        printf("\n1 - SIGUSR1\n2 - SIGINT\n3 - SIGKILL\n");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            printf("\nSIGUSR1 sent to handler\n");
            system("killall -s 10 handler");
            break;
        case 2:
            system("killall -s 2 handler");
            printf("\nSIGINT sent to handler\n");
            break;
        case 3:
        printf("\nSIGKILL sent to handler\n");
            system("killall -s 9 handler");
            break;
        default:
            break;
        }
    } while (option != 3);

    exit(0);
}
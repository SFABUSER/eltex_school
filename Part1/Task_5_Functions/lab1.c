#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 100

typedef struct
{
    char name[10];
    char second_name[10];
    char tel[10];
} abonent;

void addAbonent(abonent item[])
{
    int flag = 0;

    for (int i = 0; i < SIZE; i++)
    {
        if (item[i].name[0] == 0)
        {
            flag = 1;
            char name[10];
            char second_name[10];
            char tel[10];

            printf("\nВведите имя:\n");
            scanf("%s", name);
            printf("Введите фамилию:\n");
            scanf("%s", second_name);
            printf("Введите телефон:\n");
            scanf("%s", tel);
            
            strcpy(item[i].name, name);
            strcpy(item[i].second_name, second_name);
            strcpy(item[i].tel, tel);

            
            break;
        }
    }

    if (flag == 0)
    {
        printf("Справочник полный!");
    }

    return;
}

void deleteAbonent(abonent item[])
{
    int flag = 0;
    char nameForDelete[10];

    printf("\nВведите имя:\n");
    scanf("%s", nameForDelete);

    for (int i = 0; i < SIZE; i++)
    {
        if (item[i].name != 0)
        {
            if (strcmp(item[i].name, nameForDelete) == 0)
            {
                flag = 1;

                memset(item + i, 0, sizeof(abonent));
            }
        }
    }

    if (flag == 0)
    {
        printf("Совпадений не найдено!");
    }

    return;
}

void findAbonent(abonent item[])
{
    int flag = 0;
    char nameForFind[10];

    printf("\nВведите имя:\n");
    scanf("%s", nameForFind);

    for (int i = 0; i < SIZE; i++)
    {
        if (item[i].name != 0)
        {
            if (strcmp(item[i].name, nameForFind) == 0)
            {
                flag = 1;

                printf("\n%s\n%s\n%s\n\n", item[i].name, item[i].second_name, item[i].tel);
            }
        }
    }

    if (flag == 0)
    {
        printf("Совпадений не найдено!");
    }

    return;
}

void printAbonents(abonent item[])
{
    for (int i = 0; i < SIZE; i++)
    {
        if (item[i].name[0] != 0)
        {
            printf("\n%s\n%s\n%s\n\n", item[i].name, item[i].second_name, item[i].tel);
        }
    }

    return;
}

int main()
{
    abonent abonent_item[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        abonent_item[i].name[0] = 0;
        abonent_item[i].tel[0] = 0;
        abonent_item[i].second_name[0] = 0;
    }

    int option = 0;

    while (option != 5)
    {
        printf("Выберите опцию:\n1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            addAbonent(abonent_item);
            break;
        case 2:
            deleteAbonent(abonent_item);
            break;
        case 3:
            findAbonent(abonent_item);
            break;
        case 4:
            printAbonents(abonent_item);
            break;
        case 5:
            break;
        default:
            break;
        }
    }
}
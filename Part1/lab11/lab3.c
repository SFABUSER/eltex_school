#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SHOPS_NUM 5
#define CUSTOMERS_NUM 3
#define SHOP_MAX_CAPACITY 10000
#define CUSTOMER_MAX_DEMAND 100000
#define LOAD_SIZE 5000

int randomizer(int start, int end)
{
    return (start + rand() % (end - start + 1));
}

typedef struct
{
    int id;
    int capacity;
} shop;

typedef struct
{
    int id;
    int demand;
} customer;

shop shops[SHOPS_NUM];
customer customers[CUSTOMERS_NUM];

pthread_mutex_t busy[SHOPS_NUM];

void initShops(int start, int end)
{
    for (int i = 0; i < SHOPS_NUM; i++)
    {
        shops[i].id = i;
        shops[i].capacity = randomizer(start, end);
        printf("SHOP %d:\n CAPACITY: %d\n", shops[i].id, shops[i].capacity);
    }
    printf("\n");
}

void initCustomers(int start, int end)
{
    for (int i = 0; i < CUSTOMERS_NUM; i++)
    {
        customers[i].id = i;
        customers[i].demand = randomizer(start, end);
        printf("CUST %d:\n DEMAND: %d\n", customers[i].id, customers[i].demand);
    }
    printf("\n");
}

void *buyingProcess(void *args)
{
    // printf("%d %d\n", (*(customer *)args).id, (*(customer *)args).demand);
    int cust_id = *(int *)args;
    int exit_status;

    while (1)
    {
        printf("Потребность покупателя %d: %d\n", cust_id, customers[cust_id].demand);
        if (customers[cust_id].demand <= 0)
        {
            printf("Потребность покупателя %d закрыта, завершение потока...\n", cust_id);
            pthread_exit(&exit_status);
        }

        for (int i = 0; i < SHOPS_NUM; i++)
        {
            if (pthread_mutex_trylock(&busy[i]) == 0)
            {
                printf("Покупатель %d: зашёл в магазин %d\n", cust_id, shops[i].id);
                if (shops[i].capacity > 0)
                {
                    printf("Покупатель %d: взял в магазине %d %d товаров\n", cust_id, shops[i].id, shops[i].capacity);

                    customers[cust_id].demand = customers[cust_id].demand - shops[i].capacity;
                    shops[i].capacity = 0;
                    pthread_mutex_unlock(&busy[i]);
                    break;
                }
                else
                {
                    printf("В магазине %d пусто, иду в следующий...\n", shops[i].id);
                    pthread_mutex_unlock(&busy[i]);
                }
            }
            else
            {
                printf("Покупатель %d: магазин %d закрыт, иду в следующий...\n", (*(customer *)args).id, shops[i].id);
            }
        }
        sleep(2);
    }
}

void *loadShops()
{
    while (1)
    {
        for (int i = 0; i < SHOPS_NUM; i++)
        {
            pthread_mutex_lock(&busy[i]);
            if (shops[i].capacity <= LOAD_SIZE)
            {
                shops[i].capacity = shops[i].capacity + LOAD_SIZE;
                printf("Грузчик заполнил магазин %d на %d, кол-во товаров: %d\n", shops[i].id, LOAD_SIZE, shops[i].capacity);
            }
            pthread_mutex_unlock(&busy[i]);
            sleep(1);
        }
    }
}

int main()
{
    srand(time(NULL));

    initShops(0, SHOP_MAX_CAPACITY);
    initCustomers(SHOP_MAX_CAPACITY, CUSTOMER_MAX_DEMAND);

    pthread_t cust_threads[CUSTOMERS_NUM];
    pthread_t loader_thread;
    int status;
    int join_status;

    for (int i = 0; i < SHOPS_NUM; i++)
    {
        pthread_mutex_init(&busy[i], NULL);
    }

    for (int i = 0; i < CUSTOMERS_NUM; i++)
    {
        status = pthread_create(&cust_threads[i], NULL, buyingProcess, &customers[i].id);
        if (status != 0)
        {
            printf("Can't create thread, status = %d\n", status);
            exit(0);
        }
    }

    status = pthread_create(&loader_thread, NULL, loadShops, NULL);

    for (int i = 0; i < CUSTOMERS_NUM; i++)
    {
        status = pthread_join(cust_threads[i], &join_status);
    }

    pthread_cancel(loader_thread);
}

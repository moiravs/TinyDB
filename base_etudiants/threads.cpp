#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "threads.hpp"
#include "query.hpp"

pthread_t tid[4];

void *fonction_1(void *arguments)
{
    puts("ahhh");
    struct pipes *args = (struct pipes *)arguments;
    puts("function1 started");
    char query[256];
    char okidok[6] = "okido";
    while (true)
    {
        read(args->filed1[0], query, 6);
        close(args->filed1[0]);
        if (strcmp(query, "selec") == 0)
        {
            gestion_query(args->db, query);
            write(args->filed1[0], okidok, 6);
            close(args->filed1[0]);
        }
    }
}

void *fonction_2(void *arguments)
{
    puts("ahhh");
    struct pipes *args = (struct pipes *)arguments;
    puts("function1 started");
    char query[6];
    char okidok[6] = "okido";
    while (true)
    {
        read(args->filed2[0], query, 6);
        close(args->filed2[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args->db, query);
            write(args->filed2[0], okidok, 6);
            close(args->filed2[0]);
        }
    }
}

void *fonction_3(void *arguments)
{
    puts("ahhh");
    struct pipes *args = (struct pipes *)arguments;
    puts("function1 started");
    char query[6];
    char okidok[6] = "okido";
    while (true)
    {
        read(args->filed3[0], query, 6);
        close(args->filed3[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args->db, query);
            write(args->filed3[0], okidok, 6);
            close(args->filed3[0]);
        }
    }
}

void *fonction_4(void *arguments)
{
    puts("ahhh");
    struct pipes *args = (struct pipes *)arguments;
    puts("function1 started");
    char query[6];
    char okidok[6] = "okido";
    while (true)
    {
        read(args->filed4[0], query, 6);
        close(args->filed4[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args->db, query);
            write(args->filed4[0], okidok, 6);
            close(args->filed4[0]);
        }
    }
}

void creation_thread_and_pipes(struct pipes *args)
{
    int err;
    int fd1[2];
    pipe(fd1);
    fd1[0]; //-> for using read end
    fd1[1]; //-> for using write end
    args->filed1 = fd1;
    int fd2[2];
    pipe(fd2);
    fd2[0]; //-> for using read end
    fd2[1]; //-> for using write end
    args->filed2 = fd2;
    int fd3[2];
    pipe(fd3);
    fd3[0]; //-> for using read end
    fd3[1]; //-> for using write end
    args->filed3 = fd3;
    int fd4[2];
    pipe(fd4);
    fd4[0]; //-> for using read end
    fd4[1]; //-> for using write end
    args->filed1 = fd4;
    // spawn 4 threads:
    err = pthread_create(&(tid[1]), NULL, &fonction_1, (void *)&args);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    err = pthread_create(&(tid[2]), NULL, &fonction_2, (void *)&args);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    err = pthread_create(&(tid[3]), NULL, &fonction_3, (void *)&args);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    err = pthread_create(&(tid[4]), NULL, &fonction_4, (void *)&args);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
}


// mutex manquant

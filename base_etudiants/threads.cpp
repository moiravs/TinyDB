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
    struct pipes *args = (struct pipes *)arguments;
    puts("function1 started");
    char query[256] = "0";
    char okidok[256] = "okido";
    while (true)
    {
        puts("problem here");
        read(args->filed1[0], query, 256);
        // close(args->filed1[0]);
        if (strcmp(query, "selec") == 0)
        {
            gestion_query(args->db, query);
            write(args->filed1[1], okidok, 256);
            close(args->filed1[1]);
        }
    }
}

void *fonction_2(void *arguments)
{
    puts("ahhh1");
    struct pipes *args1 = (struct pipes *)arguments;
    puts("function2 started");
    char query[256] = "0";
    char okidok[256] = "okido";
    while (true)
    {
        puts("problem here here?");
        read(args1->filed2[0], query, 256);
        // close(args1->filed2[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args1->db, query);
            write(args1->filed2[1], okidok, 256);
            close(args1->filed2[1]);
        }
    }
}

void *fonction_3(void *arguments)
{
    puts("ahhh2");
    struct pipes *args2 = (struct pipes *)arguments;
    puts("function3 started");
    char query[256] = "0";
    char okidok[256] = "okido";
    while (true)
    {
        read(args2->filed3[0], query, 256);
        // close(args2->filed3[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args2->db, query);
            write(args2->filed3[1], okidok, 256);
            close(args2->filed3[1]);
        }
    }
}

void *fonction_4(void *arguments)
{
    puts("ahhh");
    struct pipes *args3 = (struct pipes *)arguments;
    puts("function4 started");
    char query[256] = "0";
    char okidok[256] = "okido";
    while (true)
    {
        read(args3->filed4[0], query, 256);
        // close(args3->filed4[0]);
        if (strcmp(query, "selec") == 0)
        {
            puts("ahhh ça fonctionne1");
            gestion_query(args3->db, query);
            write(args3->filed4[1], okidok, 256);
            close(args3->filed4[1]);
        }
    }
}

void creation_thread_and_pipes(struct pipes *args)
{
    int err;
    int fd1[2];
    pipe(fd1);

    args->filed1 = fd1;
    int fd2[2];
    pipe(fd2);

    args->filed2 = fd2;
    int fd3[2];
    pipe(fd3);

    args->filed3 = fd3;
    int fd4[2];
    pipe(fd4);

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

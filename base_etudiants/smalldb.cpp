#include "common.h"
#include "db.hpp"
#include "queries.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <vector>

// Permet de définir un gestionnaire de signaux pour SIGPIPE,
// ce qui évite une fermeture abrupte du programme à la réception
// du signal (utile si vous avez des opérations de nettoyage à
// faire avant de terminer le programme)
#include <signal.h>
database_t *db = new database_t;

void *work(void * socket_desc)
{
    int new_socket = *(int *)socket_desc;
    std::cout << "thread created" << std::endl;
    char *buffer = new char[2048];
    int lu;
    FILE *ush = fdopen(new_socket, "w");
    if (ush == NULL)
    {
        puts("errorfile");
    }

    while ((lu = read(new_socket, buffer, 2048)) > 0)
    {
        std::cout << "message received" << std::endl;
        std::cout << buffer << std::endl;
        // checked_wr(write(new_socket, buffer, lu));
        parse_and_execute(ush, db, buffer);
        fflush(ush);

        std::cout << "flush done" << std::endl;
    }
    fclose(ush);
    std::cout << "close done" << std::endl;
    delete[] buffer;
    std::cout << "thread finished";
    return 0;
}


int main(int argc, char const *argv[])
{
    db->path = argv[1];
    db_load(db, db->path);
    signal(SIGPIPE, SIG_IGN);
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    // Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;

    // Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(8080);

    // Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    // Bind the address struct to the socket
    checked(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));

    // Listen on the socket, with 40 max connection requests queued
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
    pthread_t tid[60];
    int i = 0;
    while (true)
    {
        // Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);

        // for each client request creates a thread and assign the client request to it to process
        // so the main thread can entertain next request
        if (pthread_create(&tid[i++], NULL, work, &newSocket) != 0)
            printf("Failed to create thread\n");

        if (i >= 50)
        {
            i = 0;
            while (i < 50)
                pthread_join(tid[i++], NULL);
            i = 0;
        }
    }
    return 0;
}

/*

struct arg_struct
{
    sockaddr_in *arg1;
    int *arg2;
};

void server(struct sockaddr_in *server_addr, int &socketDesc)
{
    memset(server_addr, (char)0, sizeof(sockaddr_in));

    socketDesc = socket(AF_INET, SOCK_STREAM, 0);

    if (socketDesc < 0)
    {
        std::cerr << "Cannot open socket" << std::endl;
        return;
    }
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(8080);
    int n;

    if ((n = bind(socketDesc, (struct sockaddr *)server_addr, sizeof(sockaddr_in))) < 0)
    {
        std::cerr << "Cannot Bind" << std::endl;
        return;
    }

    if ((n = listen(socketDesc, SOMAXCONN) < 0))
    {
        std::cerr << "Cannot Listen!" << std::endl;
        return;
    }
}
void *func(void *args)
{
    struct arg_struct *args_v2 = (struct arg_struct *)args;
    char memory[1024] = {0};
    int n;

    while ((n = recv(*args_v2->arg2, memory, 1024, 0)))
    {
        // std::cout << "Current Thread ID " << std::this_thread::get_id() << std::endl;
        std::cout << memory << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int socketDesc;
    struct sockaddr_in server_addr;
    server(&server_addr, socketDesc);
    struct arg_struct args;

    std::vector<pthread_t> poolofthread;
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(sockaddr_in);
    int new_sd;
    args.arg1 = &client_addr;
    *args.arg2 = new_sd;

    while ((new_sd = accept(socketDesc, (sockaddr *)&client_addr, (socklen_t *)&length)) > 0)
    {
        std::cout << "Client Accepted" << std::endl;
        pthread_t tid;
        pthread_create(&tid, NULL, &func, (void *)&args);
        poolofthread.push_back(tid);
    }

    for (auto &thread: poolofthread){
        pthread_join(thread, NULL);
    }
    return 0;
}
*/
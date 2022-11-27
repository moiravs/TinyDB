#include "common.h"
#include "db.hpp"
#include "signalshandler.hpp"
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
database_t db;

void *work(void *socket_desc)
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
        parse_and_execute(ush, &db, buffer);
        fflush(ush);
    }
    fclose(ush);
    std::cout << "Thread number " << new_socket << " closed" << std::endl;
    delete[] buffer;
    return 0;
}

int main(int argc, char const *argv[])
{
    db.path = argv[1];
    db_load(&db, db.path);
    signal(SIGPIPE, SIG_IGN);
    setup_principal_interrupt_handler();
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
    serverAddr.sin_port = htons(28772);

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
        std::cout << "Accepted connection number " << newSocket << std::endl;
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

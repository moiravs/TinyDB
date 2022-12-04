#include "common.h"
#include "db.hpp"
#include "signalshandler.hpp"
#include "queries.hpp"
#include "errorcodes.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <err.h> // err
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
extern database_t db;
extern std::vector<int> clientSockets;

void *work(void *socket_desc)
{
    int new_socket = *(int *)socket_desc;
    std::cout << "Thread number " << new_socket << " created" << std::endl;
    char *buffer = new char[2048];
    int lu;
    FILE *file_new_socket = fdopen(new_socket, "w");
    if (file_new_socket == NULL)
    {
        puts("errorfile");
    }

    while ((lu = checked(read(new_socket, buffer, 2048))) > 0)
    {
        parse_and_execute(file_new_socket, &db, buffer);
        fflush(file_new_socket);
    }
    fclose(file_new_socket);
    std::cout << "Thread number " << new_socket << " closed" << std::endl;
    delete[] buffer;
    return 0;
}

int main(int argc, char const *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    setup_server_interrupt_handler();
    db.path = argv[1];
    db_load(&db, db.path);
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    // Create the socket.
    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        err(SOCKET_ERROR, "Failed to create to socket");
        exit(EXIT_FAILURE);
    }

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
        err(LISTEN_ERROR, "Failed to listen");
    pthread_t tid[60];
    int i = 0;
    while (true)
    {
        // Accept call creates a new socket for the incoming connection

        addr_size = sizeof serverStorage;
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size)) > 0)
        {
            clientSockets.push_back(newSocket);
            std::cout << "Accepted connection number " << newSocket << std::endl;
            // for each client request creates a thread and assign the client request to it to process
            // so the main thread can entertain next request
            // Bloque le signal (pour le thread courant)
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            if (pthread_create(&tid[i++], NULL, work, &newSocket) != 0)
                printf("Failed to create thread\n");
            // Débloque le signal (pour le thread courant)
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
        else {
            if (errno == EINTR)
                continue;
        }

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

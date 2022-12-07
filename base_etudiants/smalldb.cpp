/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#include "common.h"
#include "db.hpp"
#include "signalshandler.hpp"
#include "queries.hpp"
#include "errorcodes.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <err.h> // err
#include <csignal>
#include <iostream>

extern database_t db;
extern std::vector<int> clientSockets;
int numberClients = 0;

void *work(void *socket_desc)
{
    int new_socket = *(int *)socket_desc;
    clientSockets.push_back(new_socket);
    std::cout << "Thread number " << new_socket << " created" << std::endl;
    char *buffer = new char[2048];
    int lu;
    FILE *file_new_socket = fdopen(new_socket, "w");
    if (file_new_socket == NULL)
        puts("errorfile");
    fputs("Connected\n", file_new_socket);
    fflush(file_new_socket);
    while ((lu = checked(read(new_socket, buffer, 2048))) > 0)
    {
        parse_and_execute(file_new_socket, &db, buffer);
        fflush(file_new_socket); // client receive the complete response and doesnt need to wait the next query
    }
    fclose(file_new_socket);
    std::cout << "Thread number " << new_socket << " closed" << std::endl;
    numberClients--;
    delete[] buffer;
    remove(clientSockets.begin(), clientSockets.end(), new_socket);
    return 0;
}

int setSocket()
{
    int serverSocket;
    struct sockaddr_in serverAddr;

    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) // Create the socket
    {
        err(SOCKET_ERROR, "Failed to create to socket");
        exit(EXIT_FAILURE);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(28772);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    checked(bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
    return serverSocket;
}

int main(int argc, char const *argv[])
{
    printf("Start\n");

    setup_principal_interrupt_handler(false); // setup signals
    if (argc < 2)
    {
        puts("Parameter of the database path is missing, exiting SmallDB");
        exit(1);
    }
    db.path = argv[1];
    db_load(&db, db.path);
    printf("DB loaded\n");
    int newSocket;
    int serverSocket = setSocket();
    if (listen(serverSocket, 20) == 0) // accept maximum 20 connections queued, further requests will be refused
        printf("Listening\n");
    else
        err(LISTEN_ERROR, "Failed to listen");
    pthread_t tid;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    while (true)
    {
        addr_size = sizeof serverStorage;
        if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size)) > 0)
        {
            if (numberClients >= MAX_CLIENT)
            {
                std::cout << "Refused connection: too many clients" << std::endl;
                FILE *file_new_socket = fdopen(newSocket, "w");
                if (file_new_socket == NULL)
                    puts("errorfile");

                fputs("Too many clients please retry later\n", file_new_socket);
                fflush(file_new_socket);
                write(newSocket, "stop", 5);
                close(newSocket);
            }
            else
            {
                numberClients++;
                std::cout << "Accepted connection number " << newSocket << std::endl;
                sigset_t mask; // Bloque le signal (pour le thread courant)
                sigemptyset(&mask);
                sigaddset(&mask, SIGUSR1 | SIGINT);
                sigprocmask(SIG_BLOCK, &mask, NULL);

                if (pthread_create(&tid, NULL, work, &newSocket) != 0)
                    printf("Failed to create thread\n");
                sigprocmask(SIG_UNBLOCK, &mask, NULL); // Débloque le signal (pour le thread courant)
            }
        }
        /*
        else
            if (errno == EINTR) // if client disconnect abruptly from server
                continue;
        */
        /*
         if (i >= 20) // if more than 20 threads
         {
             i = 0;
             puts("Too many clients. Waiting for clients to terminate");
             while (i < 20) // wait for all threads to finish before accepting new ones
                 pthread_join(tid[i++], NULL);
             i = 0;
         }
         */
    }
    return 0;
}

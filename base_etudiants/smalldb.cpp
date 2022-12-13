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
extern std::vector<int> client_sockets;
int number_clients = 0;

void *work(void *socket_desc)
{
    int new_socket = *(int *)socket_desc;
    client_sockets.push_back(new_socket);
    std::cout << "Thread number " << new_socket << " created" << std::endl;
    char *buffer = new char[2048];
    int lu;
    FILE *file_new_socket = fdopen(new_socket, "w");
    if (file_new_socket == NULL)
        puts("errorfile");
    while ((lu = checked(read(new_socket, buffer, 2048))) > 0)
    {
        parse_and_execute(file_new_socket, &db, buffer);
        fflush(file_new_socket); // client receive the complete response and doesn't need to wait the next query
        sleep(0.1);
    }
    fclose(file_new_socket);
    std::cout << "Thread number " << new_socket << " closed" << std::endl;
    number_clients--;
    delete[] buffer;
    remove(client_sockets.begin(), client_sockets.end(), new_socket);
    return 0;
}

int set_socket()
{
    int server_socket;
    struct sockaddr_in serverAddr;

    if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) // Create the socket
    {
        err(SOCKET_ERROR, "Failed to create to socket");
        exit(EXIT_FAILURE);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(28772);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    checked(bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
    return server_socket;
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
    int new_socket;
    int server_socket = set_socket();
    if (listen(server_socket, 20) == 0) // accept maximum 20 connections queued, further requests will be refused
        printf("Listening\n");
    else
        err(LISTEN_ERROR, "Failed to listen");
    pthread_t tid;
    struct sockaddr_storage server_storage;
    socklen_t addr_size;
    while (true)
    {
        addr_size = sizeof server_storage;
        if ((new_socket = accept(server_socket, (struct sockaddr *)&server_storage, &addr_size)) > 0)
        {
            if (number_clients >= MAX_CLIENT)
            {
                std::cout << "Refused connection: too many clients" << std::endl;
                FILE *file_new_socket = fdopen(new_socket, "w");
                if (file_new_socket == NULL)
                    puts("errorfile");

                fputs("Too many clients please retry later\n", file_new_socket);
                fflush(file_new_socket);
                write(new_socket, "stop", 5);
                close(new_socket);
            }
            else
            {
                number_clients++;
                std::cout << "Accepted connection number " << new_socket << std::endl;
                sigset_t mask; // Bloque le signal (pour le thread courant)
                sigemptyset(&mask);
                sigaddset(&mask, SIGUSR1 | SIGINT);
                sigprocmask(SIG_BLOCK, &mask, NULL);

                if (pthread_create(&tid, NULL, work, &new_socket) != 0)
                    printf("Failed to create thread\n");
                sigprocmask(SIG_UNBLOCK, &mask, NULL); // Débloque le signal (pour le thread courant)
            }
        }
    }
    return 0;
}

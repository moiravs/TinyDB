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
int new_socket;
database_t *db = new database_t;

void *work(void *)
{

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
}

int main(int argc, char const *argv[])
{
    pthread_t cThread;

    db->path = argv[1];
    db_load(db, db->path);
    signal(SIGPIPE, SIG_IGN);

    int server_fd = checked(socket(AF_INET, SOCK_STREAM, 0));

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    checked(bind(server_fd, (struct sockaddr *)&address, sizeof(address)));
    checked(listen(server_fd, 3));

    size_t addrlen = sizeof(address);
    new_socket = checked(accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen));
    pthread_create(&cThread, NULL, work, NULL);
    pthread_join(cThread, NULL);
    close(server_fd);
    close(new_socket);
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
    return 0;
}
*/
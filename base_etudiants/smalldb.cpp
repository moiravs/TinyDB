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
    char *buffer = new char[2048]();
    int lu;
    FILE *ush = fdopen(new_socket, "w");
    if (ush == NULL)
    {
        puts("errorfile");
    }

    while ((lu = read(new_socket, buffer, 2048)) > 0)
    {
        std::cout << "message received" << std::endl;
        // checked_wr(write(new_socket, buffer, lu));
        std::cout << buffer << std::endl;
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
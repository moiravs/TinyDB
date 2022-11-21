#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>
#include <string.h>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    // Conversion de string vers IPv4 ou IPv6 en binaire
    inet_pton(AF_INET, " 127.0.0.1 ", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    char message[] = " Salut à tous";
    write(sock, message, strlen(message));
    close(sock);
}
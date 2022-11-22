#include "common.h"
//#include "db.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

// Permet de définir un gestionnaire de signaux pour SIGPIPE,
// ce qui évite une fermeture abrupte du programme à la réception
// du signal (utile si vous avez des opérations de nettoyage à
// faire avant de terminer le programme)
#include <signal.h>

void *clientThread(void *args){
  int client_request = *((int*)args);
  int network_socket;

  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(8989);

  int connection_status = connect(network_socket, (struct sockaddr*)&server_address, sizeof(server_address));

  if (connection_status < 0){  // check for connection error
    puts("Error\n");
    return 0;
  }

  printf("Connection established\n");
  
  send(network_socket, &client_request, sizeof(client_request), 0); // send data to the socket

  close(network_socket);
  pthread_exit(NULL);
  return 0;
}


int main(int argc, char const *argv[])
{
  //  Permet que write() retourne 0 en cas de réception
  //  du signal SIGPIPE.
  signal(SIGPIPE, SIG_IGN);

  int sock = checked(socket(AF_INET, SOCK_STREAM, 0));

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);

  // Conversion de string vers IPv4 ou IPv6 en binaire
  inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

  checked(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));

  char buffer[1024];
  int longueur, i, ret;

  while (fgets(buffer, 1024, stdin) != NULL)
  {
    int i = strlen(buffer) - 1;
    buffer[i] = '\0';
    longueur = strlen(buffer) + 1;
    printf("Envoi...\n");
    checked_wr(write(sock, buffer, strlen(buffer) + 1));
    i = 0;
    while (i < longueur)
    {
      ret = read(sock, buffer, longueur - i);
      if (ret <= 0)
      {
        if (ret < 0)
          perror("read");
        else
          printf("Déconnexion du serveur.\n");
        return 1;
      }

      i += ret;
    }

    printf("Recu : %s\n", buffer);
  }

  close(sock);
  return 0;
}

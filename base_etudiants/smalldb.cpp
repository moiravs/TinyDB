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

// Permet de définir un gestionnaire de signaux pour SIGPIPE,
// ce qui évite une fermeture abrupte du programme à la réception
// du signal (utile si vous avez des opérations de nettoyage à
// faire avant de terminer le programme)
#include <signal.h>


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

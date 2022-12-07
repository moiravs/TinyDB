#include "common.h"
#include "signalshandler.hpp"
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
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

// Permet de définir un gestionnaire de signaux pour SIGPIPE,
// ce qui évite une fermeture abrupte du programme à la réception
// du signal (utile si vous avez des opérations de nettoyage à
// faire avant de terminer le programme)
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
  //  Permet que write() retourne 0 en cas de réception
  //  du signal SIGPIPE.
  if (argc != 2)
  {
    puts("Parameter IP is missing");
    exit(0);
  }
  signal(SIGPIPE, SIG_IGN);
  setup_principal_interrupt_handler();
  int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);
  // Conversion de string vers IPv4 ou IPv6 en binaire
  if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == 0)
  {
    puts("IP adress doesn't exist");
    exit(1);
  }
  checked(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));
  char buffer[80];
  const char *p = inet_ntop(AF_INET, &serv_addr.sin_addr, buffer, 80);
  if (p == NULL)
    std::cout << "IP adress is not the IP adress of the server";

  char bufferStdin[2048];
  char bufferSocket[2048];
  int longueur, i, ret;
  int lu;

  int fd = fileno(stdin);
  int flags = fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
  fcntl(sock, F_SETFL, flags);
  bool end = false;
  while (!end)
  {
    if (fgets(bufferStdin, 2048, stdin)!= NULL)
    {

      int i = strlen(bufferStdin) - 1;
      bufferStdin[i] = '\0';
      checked_wr(write(sock, bufferStdin, 2048));
    }
    if (feof(stdin)){
      end = true;
    }
    lu = read(sock, bufferSocket, 2048);
    sleep(1);
    if (lu > 0)
    {
      bufferSocket[lu] = '\0';
      if (strcmp(bufferSocket, "stop") == 0)
      {
        end = true;
      }
      printf("%s", bufferSocket);
    }
  }
  close(0);
  close(sock);
  return 0;
}

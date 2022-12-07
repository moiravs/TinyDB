#include "common.h"
#include "signalshandler.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
  //  Permet que write() retourne 0 en cas de réception
  //  du signal SIGPIPE.
  if (argc != 2)
  {
    puts("Parameter IP is missing");
    exit(1);
  }
  setup_principal_interrupt_handler(true);
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
  char bufferStdin[2048], bufferSocket[2048];
  int i, lu;
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
      bufferStdin[i] = '\0'; //put ending character
      checked_wr(write(sock, bufferStdin, 2048));
    }
    if (feof(stdin))
      end = true;
    lu = read(sock, bufferSocket, 2048);
    if (lu > 0)
    {
      bufferSocket[lu] = '\0'; 
      if (strcmp(bufferSocket, "stop") == 0)
        end = true;
      printf("%s", bufferSocket);
    }
    sleep(0.1);
  }
  close(0);
  close(sock);
  return 0;
}

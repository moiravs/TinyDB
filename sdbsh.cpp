/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#include "common.h"
#include "signalshandler.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>

int setup_socket(int argc, char const *argv[])
{
  if (argc != 2)
  {
    puts("Parameter IP is missing");
    exit(1);
  }
  setup_principal_interrupt_handler(true); // setup signals
  int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(28772);
  if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == 0)
  {
    puts("IP address doesn't exist");
    exit(1);
  }
  checked(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));
  char buffer[80];
  const char *p = inet_ntop(AF_INET, &serv_addr.sin_addr, buffer, 80); //
  if (p == NULL)
  {
    std::cout << "IP address doesn't exist";
    exit(1);
  }
  return sock;
}

int main(int argc, char const *argv[])
{

  int sock = setup_socket(argc, argv);
  char buffer_stdin[2048], buffer_socket[2048];
  int i, lu;
  int fd = fileno(stdin);
  int flags = fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags); // put non-blocking stdin
  fcntl(sock, F_SETFL, flags); // put non-blocking sock
  bool end = false;
  bool waiting_for_response = false;
  while ((!end) || (waiting_for_response))
  {
    if (fgets(buffer_stdin, 2048, stdin) != NULL)
    {
      int i = strlen(buffer_stdin) - 1;
      buffer_stdin[i] = '\0'; // put ending character
      checked_wr(write(sock, buffer_stdin, 2048));
      waiting_for_response = true;
    }
    else if (feof(stdin))
    {
      sleep(1); //to get last responses of the server
      end = true;
    }
    while ((lu = read(sock, buffer_socket, 2048)) > 0)
    {
      buffer_socket[lu] = '\0';
      if (strcmp(buffer_socket, "stop") == 0)
        end = true;
      printf("%s", buffer_socket);
      waiting_for_response = false;
    }
    sleep(0.2); // optimizes code
  }
  close(0);
  close(sock);
  return 0;
}

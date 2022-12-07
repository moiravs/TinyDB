/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#include "db.hpp"
#include "signalshandler.hpp"
#include <cstdio>
#include <csignal>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/select.h>
#include <assert.h>

database_t db;
std::vector<int> clientSockets;

static void principal_interrupt_handler(int sig)
{
   if (sig == SIGINT)
   {
      printf("\nCaught CTRL+C signal, exiting SmallDB.\n");
      close(STDIN_FILENO);
      exit(0);
   }
   else if (sig == SIGUSR1)
   {
      printf("Caught SIGUSR1, comitting database changes to the disk...\n");
      db_save(&db);
   }
}

static void server_interrupt_handler(int sig)
{
   if (sig == SIGINT)
   {
      printf("\nCaught CTRL+C signal, saving and exiting SmallDB.\n");
      db_save(&db);
      
      for (auto &client : clientSockets) // write to clients to stop
      {
         write(client, "stop", 5);
         close(client);
      }
      
      exit(0);
   }
}

void setup_principal_interrupt_handler(bool client)
{
   if (client)
      signal(SIGINT, principal_interrupt_handler); 
   else
      signal(SIGINT, server_interrupt_handler); 
   signal(SIGUSR1, principal_interrupt_handler);

   struct sigaction action;
   action.sa_handler = principal_interrupt_handler;
   action.sa_flags = 0;
   if (sigemptyset(&action.sa_mask) < 0)
      perror("sigemptyset()");
   else if (sigaction(SIGUSR1, &action, NULL) < 0)
      perror("sigaction()");
}

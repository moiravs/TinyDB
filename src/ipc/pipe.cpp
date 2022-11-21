#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>

#include "ipc.h"
#include "sighandler.h"

read_state_t read_line(int fd, char* buffer, size_t size, unsigned *i) {
   int read_val = '\0';

   for (; *i < size && read_val != '\n'; ++*i) {
      errno = 0;
      read_val = read(fd, &buffer[*i], 1);
      
      if (read_val == 0) {
         return READ_EOF;
      } else if (read_val < 0) {
         if (errno != EINTR) {
            perror("read_line()");
            return READ_ERROR;
         } else if (errno == EINTR) {
            return READ_INTERRUPTED;
         }
      }
      
      read_val = buffer[*i];
   }
   
   if (*i >= size) {
      buffer[size - 1] = '\0';
      fprintf(stderr, "[%d] data is too large to be stored in the buffer.\n", getpid());
      return READ_TOO_LARGE;
   }
   
   buffer[*i] = '\0';
   
   // Traitement supplémentaires pour les redirections de
   // stdin via des fichiers suivant la convention de fin 
   // de ligne Windows (\r\n au lieu de \n).
   if (*i >= 2 && buffer[*i - 2] == '\r') {
      --*i;
      buffer[*i] = '\0', buffer[*i - 1] = '\n';
   }
   
   return READ_OK;
}

void close_main_process_pipes(const dedicated_process_t processes[], unsigned n) {
   for (unsigned i = 0; i < n; ++i) {
      close(processes[i].fd_command[1]);
      close(processes[i].fd_transaction[0]);
   }
}

bool send_all_command(const char* cmd, const dedicated_process_t processes[], unsigned n) {
   size_t size = strlen(cmd);

   for (unsigned i = 0; i < n; ++i) {
      if (processes[i].fd_command[1] != -1) {
         if (!send_command(&processes[i], cmd, size))
            return 0;
      }
   }
   
   return 1;
}

void notify_sync_done(const dedicated_process_t* process) {
   char buffer = TRANSACTION_OK;
   int ret;
   do {
      errno = 0;
      ret = write(process->fd_transaction[1], &buffer, 1);
   } while (ret < 0 && errno == EINTR);
}

void wait_end_sync(const dedicated_process_t processes[]) {
   char buffer;
   int ret;
   for (int i = 0; i < LAST_PROCESS_TYPE + 1; ++i) {
      // Attendre jusqu'à lire un char ou avoir un SIGPIPE
      // Les autres erreurs (sauf réception d'autres signaux)
      // sont font ignorer
      do {
         errno = 0;
         ret = read(processes[i].fd_transaction[0], &buffer, 1);
      } while (ret < 0 && errno == EINTR);
      
      if (ret < 0 && errno != EINTR) {
         perror("transaction pipe reading");
      }
   }
}

bool send_command(const dedicated_process_t* destination, const char* message, size_t size) {
   int ret, remaining = size;
   do {
      errno = 0;
      ret = write(destination->fd_command[1], &message[size - remaining], remaining);
      if (ret > 0) {
         remaining -= ret;
      }
   } while ((ret < 0 && errno == EINTR) || (ret > 0 && remaining > 0));
   
   return ret != 0;
}


/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/
#include "process.hpp"
#include "db.hpp"
#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>

database_t *db;
const char *db_path;
int fdInsert[2], fdSelect[2], fdUpdate[2], fdDelete[2], fdResponse[2];

static volatile int keepRunning = 1;
pid_t child_select = -1;
pid_t child_insert = -1;
pid_t child_delete = -1;
pid_t child_update = -1;

int operationInProgress = 0;

void close_application(bool force)
{
  char *status1 = new char[256];

  if (!force)
  {
    puts("Waiting for requests to terminate");
    close(fdResponse[1]);
    while (operationInProgress > 0)
    {
      while (read(fdResponse[0], status1, 256) > 0)
      {
        operationInProgress--;
      }
      if (operationInProgress > 0)
      {
        std::cout << operationInProgress << " operations in progress: Wait" << std::endl;
        sleep(1);
      }
    }
  }

  puts("Committing database changes to the disk...");
  db->db_save(db_path);
  char kill[256] = "KILL";

  if (!force)
  {

    close(fdSelect[0]);
    write(fdSelect[1], kill, 256);
    close(fdUpdate[0]);
    write(fdUpdate[1], kill, 256);
    close(fdDelete[0]);
    write(fdDelete[1], kill, 256);
    close(fdInsert[0]);
    write(fdInsert[1], kill, 256);
    int wstatus;
    waitpid(child_delete, &wstatus, 0);
    waitpid(child_select, &wstatus, 0);
    waitpid(child_insert, &wstatus, 0);
    waitpid(child_update, &wstatus, 0);
  }

  puts("Done");
}
void signal_handling(int signum)
{
  close_application(signum != 2);
  kill(getpid(), SIGKILL);
}

int main(int argc, char const *argv[])
{

  db_path = argv[1];

  db = (database_t *)mmap(NULL, (sizeof(database_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  db->db_init();
  std::cout << "Loading your tiny tiny database..." << std::endl;
  db->db_load(db_path);
  std::cout << "Done !" << std::endl;
  // fd : file descriptor (fd0 : standard input (stdin), fdInsert : standard output (stdout), fd2 : standard error (stderr))
  // pipe(fd) : creates both the reading and writing ends of the pipe
  // fd[0] : standard input, fd[1] : standard output
  pipe(fdInsert);
  pipe(fdSelect);
  pipe(fdUpdate);
  pipe(fdDelete);
  pipe(fdResponse);

  // no blocking pipe for response
  fcntl(fdResponse[0], F_SETFL, fcntl(fdResponse[0], F_GETFL) | O_NONBLOCK);

  char query[256] = "0";

  child_select = fork(); // create child
  if (child_select < 0)
  {
    perror("fork error");
    abort();
  }
  if (child_select == 0)
  {
    process_select();
    exit(0);
  }

  child_insert = fork();
  if (child_insert < 0)
  {
    perror("fork error");
  }
  if (child_insert == 0)
  {
    process_insert();
    exit(0);
  }
  child_update = fork();
  if (child_update < 0)
  {
    perror("fork error");
  }
  if (child_update == 0)
  {
    process_update();
    exit(0);
  }
  child_delete = fork();
  if (child_delete < 0)
  {
    perror("fork error");
  }
  if (child_delete == 0)
  {
    process_delete();
  }
  signal(SIGINT, signal_handling);  // handles the signal Ctrl + C and terminates program
  signal(SIGUSR1, signal_handling); // handles abnormal program termination

  bool transaction = false;
  char *status1 = new char[256];
  while (!std::cin.eof())
  {
    while (std::cin.getline(query, 256))
    {
      close(fdResponse[1]);
      while (read(fdResponse[0], status1, 256) > 0)
      {
        operationInProgress--;
      }
      if (operationInProgress > 0)
      {
        std::cout << operationInProgress << " operations in progress: no wait" << std::endl;
      }
      query[strcspn(query, "\n")] = 0;
      char *querymod = new char[256];
      memcpy(querymod, query, 256);
      char *saveptr;
      const char *queryKey = new char[6]();
      queryKey = strtok_r(querymod, " ", &saveptr);

      if (strcmp(query, "transaction") == 0)
      {
        close(fdResponse[1]);
        while (operationInProgress > 0)
        {
          while (read(fdResponse[0], status1, 256) > 0)
          {
            operationInProgress--;
          }
          if (operationInProgress > 0)
          {
            std::cout << operationInProgress << " operations in progress: Wait" << std::endl;
            sleep(1);
          }
        }
        transaction ^= transaction;
      }
      else if (strcmp(queryKey, "select")==0)
      {
        operationInProgress++;
        close(fdSelect[0]);
        write(fdSelect[1], query, 256);
      }
      else if (strcmp(queryKey, "insert")==0)
      {
        operationInProgress++;
        close(fdInsert[0]);
        write(fdInsert[1], query, 256);
      }
      else if (strcmp(queryKey, "update")==0)
      {
        operationInProgress++;
        close(fdUpdate[0]);
        write(fdUpdate[1], query, 256);
      }
      else if (strcmp(queryKey, "delete")==0)
      {
        operationInProgress++;
        close(fdDelete[0]);
        write(fdDelete[1], query, 256);
      }
      else
      {
        puts("Bad query");
      }

      if (transaction == true)
      {
        close(fdResponse[1]);
        while (operationInProgress > 0)
        {
          std::cout << operationInProgress << " operations in progress: Wait" << std::endl;
          if (read(fdResponse[0], status1, 256) > 0)
          {
            operationInProgress--;
          }
          sleep(1);
        }
      }
    }
  }
  close_application(false);
}

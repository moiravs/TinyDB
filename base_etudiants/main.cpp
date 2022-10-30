#include <stdio.h>
#include "db.hpp"
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/mman.h>
#include <fstream>

static volatile int keepRunning = 1;
pid_t child_select = -1;
pid_t child_insert = -1;
pid_t child_delete = -1;
pid_t child_update = -1;
database_t *db;
const char *db_path;
int fdInsert[2], fdSelect[2], fdUpdate[2], fdDelete[2], fdResponse[2];

void close_application(bool force)
{

  if (!force)
  {
    puts("Waiting for requests to terminate");
    close(fdSelect[0]);
    write(fdSelect[1], "KILL", 256);
    close(fdUpdate[0]);
    write(fdUpdate[1], "KILL", 256);
    close(fdDelete[0]);
    write(fdDelete[1], "KILL", 256);
    close(fdInsert[0]);
    write(fdInsert[1], "KILL", 256);

    int wstatus;
    waitpid(-1, &wstatus, 0);
  }
  puts("Committing database changes to the disk...");
  db_save(db, db_path);
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
  db_init(db);
  std::cout << "Loading your tiny tiny database..." << std::endl;
  db_load(db, db_path);
  std::cout << "Done !" << std::endl;
  // fd : file descriptor (fd0 : standard input (stdin), fdInsert : standard output (stdout), fd2 : standard error (stderr))
  // pipe(fd) : creates both the reading and writing ends of the pipe
  // fd[0] : standard input, fd[1] : standard output
  pipe(fdInsert);
  pipe(fdSelect);
  pipe(fdUpdate);
  pipe(fdDelete);
  pipe(fdResponse);

  char query[256] = "0";

  child_select = fork();
  if (child_select < 0)
  {
    perror("fork error");
    abort();
  }
  if (child_select == 0)
  {
    char query[256] = "01";
    close(fdResponse[0]);
    write(fdResponse[1], "SUCCESS", 256);
    bool killed = false;
    while (!killed)
    {
      close(fdSelect[1]);            // close the writing end of the pipe
      read(fdSelect[0], query, 256); // reads 256 bytes into memory area indicated by query
      if (strcmp(query, "01") != 0) // if query changed
      {
        char *querymod = new char[256]; // create a new modifiable string
        memcpy(querymod, query, 256);   // save query to querymod
        char *saveptr;
        const char *queryKey = new char[6]();         // variable that'll hold the first keyword of the query (delete, insert, ...)
        queryKey = strtok_r(querymod, " ", &saveptr); // write the first word to queryKey

        if (strcmp(queryKey, "KILL") == 0)
        {
          killed = true;
        }
        else if (strcmp(queryKey, "select") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_select_and_delete(db, queryResult, query, saveptr, "select");

          close(fdResponse[0]);
          write(fdResponse[1], "SUCCESS", 256);
          delete queryResult;
        }
        memcpy(query, "01", 256); // change the query back to 01
      }
    }
    exit(0);
  }

  child_insert = fork();
  if (child_insert < 0)
  {
    perror("fork error");
  }
  if (child_insert == 0)
  {
    char query[256] = "01";
    close(fdResponse[0]);
    write(fdResponse[1], "SUCCESS", 256);

    bool killed = false;
    while (!killed)
    {
      close(fdInsert[1]);
      read(fdInsert[0], query, 256);
      if (strcmp(query, "01") != 0)
      {
        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);
        if (strcmp(queryKey, "KILL") == 0)
        {
          killed = true;
        }
        else if (strcmp(queryKey, "insert") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_insert(db, queryResult, query, saveptr);
          close(fdResponse[0]);
          write(fdResponse[1], "SUCCESS", 256);
          delete queryResult;
        }
        memcpy(query, "01", 256);
      }
    }
    exit(0);
  }
  child_update = fork();
  if (child_update < 0)
  {
    perror("fork error");
  }
  if (child_update == 0)
  {
    char query[256] = "01";
    close(fdResponse[0]);
    write(fdResponse[1], "SUCCESS", 256);
    bool killed = false;
    while (!killed)
    {
      close(fdUpdate[1]);
      read(fdUpdate[0], query, 256);
      if (strcmp(query, "01") != 0)
      {

        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);
        if (strcmp(queryKey, "KILL") == 0)
        {
          killed = true;
        }

        else if (strcmp(queryKey, "update") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_update(db, queryResult, saveptr, query);
          close(fdResponse[0]);
          write(fdResponse[1], "SUCCESS", 256);
          delete queryResult;
        }

        memcpy(query, "01", 256);
      }
    }
    exit(0);
  }
  child_delete = fork();
  if (child_delete < 0)
  {
    perror("fork error");
  }
  if (child_delete == 0)
  {
    char query[256] = "01";
    close(fdResponse[0]);
    write(fdResponse[1], "SUCCESS", 256);

    bool killed = false;
    while (!killed)
    {
      close(fdDelete[1]);
      read(fdDelete[0], query, 256);
      if (strcmp(query, "01") != 0)
      {

        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);
        if (strcmp(queryKey, "KILL") == 0)
        {
          killed = true;
        }

        else if (strcmp(queryKey, "delete") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_select_and_delete(db, queryResult, query, saveptr, "delete");
          close(fdResponse[0]);
          write(fdResponse[1], "SUCCESS", 256);
          delete queryResult;
        }
        memcpy(query, "01", 256);
      }
    }
    exit(0);
  }
  signal(SIGINT, signal_handling);  // handles the signal Ctrl + C and terminates program
  signal(SIGUSR1, signal_handling); // handles abnormal program termination

  bool transaction = false;
  char *status1 = new char[256];
  while (!std::cin.eof())
  {
    while (std::cin.getline(query, 256))
    {
      //(fgets(query, sizeof(query), stdin)){
      query[strcspn(query, "\n")] = 0;
      if (strcmp(query, "transaction") == 0)
      {
        transaction ^= transaction;
      }
      else
      {
        close(fdSelect[0]);
        write(fdSelect[1], query, 256);
        close(fdInsert[0]);
        write(fdInsert[1], query, 256);
        close(fdUpdate[0]);
        write(fdUpdate[1], query, 256);
        close(fdDelete[0]);
        write(fdDelete[1], query, 256);
      }

      if (transaction == true)
      {
        close(fdResponse[1]);
        read(fdResponse[0], status1, 256);
      }
    }
  }
  close_application(false);
}

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
#include <thread>             
#include <mutex>              
#include <condition_variable>
#include <string>
#include <string.h>
#include <sys/mman.h>

static volatile int keepRunning = 1;
pid_t child_select = -1;
pid_t child_insert = -1;
pid_t child_delete = -1;
pid_t child_update = -1;

void signal_handling(int signum)
{
  printf("Programme terminé");
  keepRunning = 0;
  kill(child_insert, SIGKILL);
  kill(child_insert, SIGKILL);
  kill(child_delete, SIGKILL);
  kill(child_update, SIGKILL);
}

void *create_shared_memory(size_t size)
{
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(int argc, char const *argv[])
{
  const char *db_path = argv[1];

  database_t *db = (database_t *)create_shared_memory(sizeof(database_t));
  db_init(db);
  std::cout << "Loading your tiny tiny database..." << std::endl;
  db_load(db, db_path);
  std::cout << "Done !" << std::endl;
  // fd : file descriptor (fd0 : standard input (stdin), fd1 : standard output (stdout), fd2 : standard error (stderr))
  // pipe(fd) : creates both the reading and writing ends of the pipe
  // fd[0] : standard input, fd[1] : standard output
  int fd1[2];
  pipe(fd1);  
  int fd2[2];
  pipe(fd2);
  int fd3[2];
  pipe(fd3);
  int fd4[2];
  pipe(fd4);
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
    while (true)
    {
      close(fd2[1]);  // close the writing end of the pipe
      read(fd2[0], query, 256);  // reads 256 bytes into memory area indicated by query
      if (strcmp(query, "01") != 0)  // if query changed
      {
        query_result_t *queryResult = new query_result_t();
        query_result_init(queryResult, query);

        char *querymod = new char[256]; // create a new modifiable string
        memcpy(querymod, query, 256);  // save query to querymod
        char *saveptr;
        const char *queryKey = new char[6](); // variable that'll hold the first keyword of the query (delete, insert, ...)
        queryKey = strtok_r(querymod, " ", &saveptr); // write the first word to queryKey

        if (strcmp(queryKey, "select") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_select_and_delete(db, query, saveptr, "select");
        }
        memcpy(query, "01", 256);  // change the query back to 01
      }
      sleep(2);
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
    

    while (true)
    {
      close(fd1[1]);
      read(fd1[0], query, 256);
      if (strcmp(query, "01") != 0)
      {
        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "insert") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_insert(db, query, saveptr);
        }
        memcpy(query, "01", 256);
      }
      sleep(2);
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
  
    while (true)
    {
      close(fd3[1]);
      read(fd3[0], query, 256);
      if (strcmp(query, "01") != 0)
      {

        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "update") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_update(db, saveptr, query);
        }

        memcpy(query, "01", 256);
      }
      sleep(2);
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
  
    while (true)
    {
      close(fd4[1]);
      read(fd4[0], query, 256);
      if (strcmp(query, "01") != 0)
      {

        char *querymod = new char[256];
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6]();
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "delete") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_select_and_delete(db, query, saveptr, "delete");
        }
        memcpy(query, "01", 256);
      }
      sleep(2);
    }
    exit(0);
  }

  while (true)
  {
    signal(SIGINT, signal_handling); // handles the signal Ctrl + C and terminates program
    signal(SIGUSR1, signal_handling); // handles abnormal program termination
    
    while (fgets(query, sizeof(query), stdin)){
      printf("query: %s\n", query);
      close(fd2[0]);
      write(fd2[1], query, 256);
      close(fd1[0]);
      write(fd1[1], query, 256);
      close(fd3[0]);
      write(fd3[1], query, 256);
      close(fd4[0]);
      write(fd4[1], query, 256);}
  }
}

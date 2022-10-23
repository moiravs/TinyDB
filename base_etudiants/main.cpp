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
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <string>
#include <string.h>
#include <sys/mman.h>

static volatile int keepRunning = 1; // jsp ce que c'est volatile ct dans stackoverflow
pid_t child_select = -1;
pid_t child_insert = -1;
pid_t child_delete = -1;
pid_t child_update = -1;

void gere_signal(int signum)
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
// mremap
int main(int argc, char const *argv[])
{
  const char *db_path = argv[1];

  database_t *db = (database_t *)create_shared_memory(sizeof(database_t));
  db_init(db);
  db_load(db, db_path);
  // int err;
  int fd1[2], fd2[2], fd3[2], fd4[2];
  pipe(fd1);
  pipe(fd2);
  pipe(fd3);
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
    printf("select process:%d\n", getpid());

  child_insert = fork();
  {
    perror("fork error");
  }
  if (child_insert == 0)
  {
    printf("insert process:%d\n", getpid());
    char query[256] = "01";
    puts("here");

    while (true)
    {
      close(fd1[1]);
      read(fd1[0], query, 256);
      if (strcmp(query, "01") != 0)
      {
        query_result_t *queryResult = new query_result_t();
        query_result_init(queryResult, query);
        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "insert") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_insert(db, query, saveptr);}
        memcpy(query, "01", 256);
        }
      sleep(2);
      printf("t\n");
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
    printf("update process:%d\n", getpid());
    {
      close(fd3[1]);
      if (strcmp(query, "01") != 0)
      {

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, query, 256);
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
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
      printf("t\n");
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
    printf("delete process:%d\n", getpid());

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "delete") == 0)
        {
          query_result_t *queryResult = new query_result_t();
          query_result_init(queryResult, query);
          query_select_and_delete(db, query, saveptr, "delete");}
        memcpy(query, "01", 256);
        }
      sleep(2);
      printf("t\n");
    }
    exit(0);
  }
  printf("main process:%d\n", getpid());
  while (true)
  {
    signal(SIGINT, gere_signal); // gere le signal genre ctrl c
    signal(SIGUSR1, gere_signal);
    printf("command:");
    std::cin.getline(query, sizeof(query));
    close(fd2[0]);
    write(fd2[1], query, 256);
    close(fd1[0]);
    write(fd1[1], query, 256);
    close(fd3[0]);
    write(fd3[1], query, 256);
    close(fd4[0]);
    write(fd4[1], query, 256);

    /*
    while (n > 0)
      pid_t pid;
    int n = 2;

    pid = wait(&status);
    printf("Child PID %ld exit with status%d\n", (long)pid, status);
    n--;
  }
    */
  }
}

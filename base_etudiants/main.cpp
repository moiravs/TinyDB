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
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, -1, 0);
}
// mremap
int main(int argc, char const *argv[])
{
  const char *db_path = argv[1];

  database_t *db = (database_t *)create_shared_memory(sizeof(database_t));
  db_init(db);

  db_load(db, db_path);
  /*
  database_t *shmem = (database_t *)create_shared_memory(db.psize);
  memcpy(shmem, db.data, db.psize);
  */
  // int err;
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
    printf("select process:%d\n", getpid());
    char jsp[256] = "01";
    puts("here");

    while (true)
    {
      close(fd2[1]);
      read(fd2[0], jsp, 256);
      if (strcmp(jsp, "01") != 0)
      {

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, jsp, 256);
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok(querymod, " ");

        if (strcmp(queryKey, "select") == 0)
        {

          gestion_query(db, jsp, queryKey);
          puts("ahhh ça fonctionne2");
        }
        memcpy(jsp, "01", 256);
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
    printf("insert process:%d\n", getpid());
    char jsp[256] = "01";
    puts("here");

    while (true)
    {
      close(fd1[1]);
      read(fd1[0], jsp, 256);
      if (strcmp(jsp, "01") != 0)
      {

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, jsp, 256);
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "insert") == 0)
        {

          gestion_query(db, jsp, queryKey);
          puts("ahhh ça fonctionne2");
        }
        memcpy(jsp, "01", 256);
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
    char jsp[256] = "01";
    puts("here");

    while (true)
    {
      close(fd3[1]);
      read(fd3[0], jsp, 256);
      if (strcmp(jsp, "01") != 0)
      {

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, jsp, 256);
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "update") == 0)
        {

          gestion_query(db, jsp, queryKey);
          puts("ahhh ça fonctionne2");
        }
        memcpy(jsp, "01", 256);
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
    char jsp[256] = "01";
    puts("here");

    while (true)
    {
      close(fd4[1]);
      read(fd4[0], jsp, 256);
      if (strcmp(jsp, "01") != 0)
      {

        char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
        memcpy(querymod, jsp, 256);
        char *saveptr;
        const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
        queryKey = strtok_r(querymod, " ", &saveptr);

        if (strcmp(queryKey, "delete") == 0)
        {

          gestion_query(db, jsp, queryKey);
          puts("ahhh ça fonctionne2");
        }
        memcpy(jsp, "01", 256);
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

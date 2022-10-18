#include <stdio.h>
#include <iostream>
#include "db.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include "query.hpp"

static volatile int keepRunning = 1; // jsp ce que c'est volatile ct dans stackoverflow

void gere_signal(int signum)
{
  printf("Programme terminé");
  keepRunning = 0;
}

int main(int argc, char const *argv[])
{
  const char *db_path = argv[1];
  database_t db;
  signal(SIGINT, gere_signal); // gere le signal genre ctrl c
  signal(SIGUSR1, gere_signal);
  db_init(&db);
  db_load(&db, db_path);
  database_t *ptrtest = &db;
  // creation des 4 enfants (je pense pas il faut le faire ici )
  /*
  for (int i = 0; i < 4; i++)
  {
    if (fork() == 0)
    {
      printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
      exit(0);
    }
  }
  */
  //char* query;

  //query_result_t *queryresultt;
  while (keepRunning){
    //std::cin >> query;
    char buffer[60];
    student_to_str(buffer, &(ptrtest->data[1340]));
    //query_result_init(queryresultt, query);
    keepRunning =0;
  } 
  //db_save(&db, db_path);
  printf("Bye bye!\n");
  return 0;
}
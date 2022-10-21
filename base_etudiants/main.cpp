#include <stdio.h>
#include <string>
#include <iostream>
#include "db.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include "threads.hpp"

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
  db_init(&db);
  db_load(&db, db_path);
  char query[sizeof(student_t)] = "0";
  struct pipes args;
  creation_thread_and_pipes(&args);
  args.db = &db;
  puts("bribriaaa");
  // création threads
  while (keepRunning)
  {
    signal(SIGINT, gere_signal); // gere le signal genre ctrl c
    signal(SIGUSR1, gere_signal);
    std::cin.getline(query, sizeof(query));
    if (strcmp(query,"0") == 0){
      char *querymod = new char[sizeof(student_t)]; // créer un nv string modifiable car strtok modifie les strings
      memcpy(querymod, query, sizeof(student_t));
      const char *queryKey = new char[6]();
      queryKey = strtok(querymod, " ");
    
    if (strcmp(queryKey, "insert") == 0)
    {
      puts("bruh");
      char * afairepasser = new char[256]();
      memcpy(afairepasser, query, sizeof(student_t));
      write(args.filed1[1], afairepasser, 256);
      close(args.filed1[1]);
    }
    else if (strcmp(queryKey, "update") == 0)
    {
      puts("bruh");
      char *afairepasser = new char[256]();
      memcpy(afairepasser, query, sizeof(student_t));
      write(args.filed2[1], afairepasser, 256);
      close(args.filed2[1]);
    }
    else if (strcmp(queryKey, "select") == 0)
    {
      puts("bruh");
      char *afairepasser = new char[256]();
      memcpy(afairepasser, query, sizeof(student_t));
      write(args.filed3[1], afairepasser, 256);
      close(args.filed3[1]);
    }
    else if (strcmp(queryKey, "delete") == 0)
    {
      puts("bruh");
      char *afairepasser = new char[256]();
      memcpy(afairepasser, query, sizeof(student_t));
      write(args.filed4[1], afairepasser, 256);
      close(args.filed4[1]);
    }
    else
    {
      puts("wtf");
    }}
  }

  // db_save(&db, "test.txt");
  printf("Bye bye!\n");
  return 0;
}
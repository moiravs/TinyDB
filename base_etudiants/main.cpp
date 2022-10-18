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
  char query[256] = "0";
  query_result_t *queryresultt = new query_result_t;
  while (keepRunning){
    //std::cin.getline(query, sizeof(query));
    strcpy(query, "insert Emeline Lecomte 6 chemistry 15/04/1993");
    char buffer[60];
    std::cout << query;
    student_to_str(buffer, &(ptrtest->data[1340]));
    query_result_init(queryresultt, query);
    delete queryresultt;
    keepRunning = 0;}
  /*
  char commande[200] = "";
  // Lire l'entrée
  while (fgets(commande, sizeof(commande), stdin) != NULL)
  {
    // Supprimer le retour à la ligne
    size_t len = strlen(commande);
    commande[len - 1] = '\0';

    pid_t pid = fork();
    if (pid > 0)
    { // Père
      if (waitpid(pid, NULL, 0) < -1)
      {
        perror("waitpid()");
        return 1;
      }
    }
    else if (pid == 0)
    { // Fils
      int r = 0;
      if (strcmp(commande, "ls") == 0){r = execlp("ls", "ls", "-l", "-a", NULL);}
      else if (strcmp(commande, "date") == 0){r = execlp("date", "date", "+%H:%M:%S", NULL);}
      else{
        printf("Vous n'avez pas entré une valeur correcte !\n");
        exit(0);}
      if (r < 0)
      {
        perror("execlp()");
        return 1;
      }
    }
    else
    { // Erreur
      perror("fork()");
      return 1;
    }
  }*/
  //db_save(&db, db_path);
  printf("Bye bye!\n");
  return 0;
}
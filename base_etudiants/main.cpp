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

static volatile int keepRunning = 1; // jsp ce que c'est volatile ct dans stackoverflow

void gere_signal(int signum)
{
  printf("Programme terminé");
  keepRunning = 0;
}

void gestion_query(database_t *db, char *query)
{
  query_result_t *queryResult = new query_result_t();
  query_result_init(queryResult, query);
  char *querymod = new char[sizeof(student_t)]; // créer un nv string modifiable car strtok modifie les strings
  memcpy(querymod, query, sizeof(student_t));
  char *saveptr;
  const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
  queryKey = strtok_r(querymod, " ", &saveptr);
  memcpy(queryResult->query, queryKey, 6);
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64](), *value = new char[64];
  char value_str[64] = "0";
  char date_str[64] = "0";

  student_t *s = new student_t;
  if (strcmp(queryKey, "insert") == 0)
  {
    if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate))
    {
      db_add(db, *s);
    }
    else
    {
      std::cout << "An error has occurred during the insert query." << std::endl;
    }
  }
  else if (strcmp(queryKey, "update") == 0 && parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue))
  {
  }
  else if (strcmp(queryKey, "select") == 0 && parse_selectors(saveptr, fieldFilter, value))
  {
    query_select_and_delete(db, queryResult, fieldFilter, value_str, value, s, date_str, queryKey);
    /*for (size_t i = 0; i < db->lsize; i++)
    {
      *s = db->data[i];
      if (strcmp(fieldFilter, "id") == 0)
      {
        sprintf(value_str, "%u", s->id); // convertir le id (unsigned) à un char* pour la comparaison
        if (strcmp(value_str, value) == 0)
        {
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "fname") == 0)
      {
        if (strcmp(s->fname, value) == 0)
        {
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "lname") == 0)
      {
        if (strcmp(s->lname, value) == 0)
        {
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "section") == 0)
      {
        if (strcmp(s->section, value) == 0)
        {
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "birthdate") == 0)
      {
        strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
        if (strcmp(date_str, value) == 0)
        {
          query_result_add(queryResult, *s);
        }
      }
      else {
        std::cout << "An error has occurred during the select query : bad filter." << std::endl;
        //break;
      }
    }*/
  }
  
  else if (strcmp(queryKey, "delete") == 0 && parse_selectors(saveptr, fieldFilter, value))
  {
    query_select_and_delete(db, queryResult, fieldFilter, value_str, value, s, date_str, queryKey);
    /*for (size_t i = 0; i < db->lsize; i++)
    {
      *s = db->data[i];
      if (strcmp(fieldFilter, "id") == 0)
      {
        sprintf(value_str, "%u", s->id); // convertir le id (unsigned) à un char* pour la comparaison
        if (strcmp(value_str, value) == 0)
        {
          //db_delete(db, i);
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "fname") == 0)
      {
        if (strcmp(s->fname, value) == 0)
        {
          //db_delete(db, i);
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "lname") == 0)
      {
        if (strcmp(s->lname, value) == 0)
        {
          //db_delete(db, i);
          query_result_add(queryResult, *s);
        }
      }
      else if (strcmp(fieldFilter, "section") == 0)
      {
        if (strcmp(s->section, value) == 0)
        {
          //db_delete(db, i);
          query_result_add(queryResult, *s);
          std::cout << "waouuu";
        }
      }

      else if (strcmp(fieldFilter, "birthdate") == 0)
      {
        strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
        if (strcmp(date_str, value) == 0)
        {
          //db_delete(db, i);
          query_result_add(queryResult, *s);
        }
      }
      else
      {
        std::cout << "An error has occurred during the delete query : bad filter." << std::endl;
        //break;
      }
    }*/
  }
  queryResult->status = QUERY_SUCCESS;
  
  
  if (queryResult->lsize > 0){
    for (size_t i = 0; i < queryResult->lsize; i++)
    {
      char buffer[sizeof(student_t)] = "0";
      student_to_str(buffer, &queryResult->students[i]);
      std::cout << buffer;
    }
  }
  delete s;
  delete queryResult;
}

int main(int argc, char const *argv[])
{
  const char *db_path = argv[1];
  database_t db;
  db_init(&db);
  db_load(&db, db_path);
  char query[sizeof(student_t)] = "0";
  while (keepRunning)
  {
    signal(SIGINT, gere_signal); // gere le signal genre ctrl c
    signal(SIGUSR1, gere_signal);
    std::cin.getline(query, sizeof(query));
    if (strcmp(query, "0") != 0)
    {
      gestion_query(&db, query);
    }
    keepRunning = 1;
  }
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
  // db_save(&db, "test.txt");
  printf("Bye bye!\n");
  return 0;
}
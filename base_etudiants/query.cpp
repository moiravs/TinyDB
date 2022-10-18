#include "query.hpp"
#include "parsing.hpp"
#include <iostream>
#include <time.h>

void query_result_init(query_result_t *result, const char *query)
{
  
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  char *queryKey; // premier mot de la query (insert, delete, ...)
  queryKey = strtok(result->query, " ");
  student_t *s{NULL};
  //  TODO insert <id> <fname><lname><section><birthdate>.Cette requête insère un nouvel étudiant dans la base de données en vérifiant que l’ID n’existe pas déjà.Si l’id existe déjà,l’insertion échoue.
  if (strcmp(query, "insert"))
  {
    std::cout << "wtf ça fonctionne";
    s->id = atoi(strtok(NULL, " "));
    *s->fname = *strtok(NULL, " ");
    *s->lname = *strtok(NULL, " ");
    *s->section = *strtok(NULL, " ");
    strptime(strtok(NULL, " "), "%Y-%m-%d", &s->birthdate);
    if (parse_insert(queryKey, s->fname, s->lname, &s->id, s->section, &s->birthdate))
    {
      query_result_add(result, *s);
      }
      else
      {
        perror("Wrong parameters parsed.");
      } // indiquer au user qu'il a introduit de mauvais paramètres
    }
  
  /*
  // TODO update < filtre >= <valeur> set < champ_modifie >= <valeur_modifiee>.Cette requête modifie tous les étudiants correspondant au filtre < filtre >= <valeur>, en donnant la valeur<valeur_modifiee> au champ<champ_modifie>.
  if (queryKey == "update")
  {
    student_t *s;
    {
      strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "),
          strtok(NULL, " "), strtok(NULL, " ");
    }; // initialisation de s
    s->id = atoi(strtok(NULL, " "));
    s->fname = strtok(NULL, " ");
    for (int i = 0; i < result->lsize; i++)
    {
      if (&result->students[i]->filtre == valeur and parse_update(char *query, char *field_filter, char *value_filter, char *field_to_update, char *update_value))){}
    }*/

  // bool parse update

/*
  // TODO delete < champ >= <valeur>.Cette requête supprime tous les étudiants qui correspondent au filtre donné.
  if (queryKey == "delete")
  {
      for
        student in result->students // pr chaque étudiant
            if (student->champ == valeur)
        {
          if (parse_delete(paramètres))
          {
            query_result_delete(result, student_t s);
          }
          else
          {
            // indiquer au user que c'est faux
          }
        }
  }*/
/*
  // TODO select < champ >= <valeur>.Cette requête renvoie la liste des étudiants qui correspondent à l’unique filtre < champ >= <valeur>.
  if (queryKey == "select")
  {
      for
        student in result->students
        {
          if (student->champ == valeur)
            if (parse_select(paramètres))
            {
              query_result_select(result, student_t s);
            }
            else
            {
              // indiquer au user que c'est faux
            }
        }
  }*/
// TODO transaction.Ce mot - clef déclare le début d’une nouvelle transaction.Plus de détails sont donnés dans la Section
  /*
  if (queryKey == "transaction")
  {
    if (parse_transaction(paramètres))
    {
      query_result_transaction(result, student_t s);
    }
    else
    {
      // indiquer au user que c'est faux
    }
  }*/
}

void query_result_add(query_result_t *result, student_t s)
{
}
/*
void query_result_delete(query_result_t * result, student_t s)
{
}
void query_result_select(query_result_t * result, student_t s)
{
}
void query_result_transaction(query_result_t * result, student_t s)
{
}
void query_result_update(query_result_t * result, student_t s)
{
  student->champmodifié = valeurmodifiéé
}
*/
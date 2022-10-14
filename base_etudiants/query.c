#include "query.h"

#include <time.h>

void query_result_init(query_result_t *result, const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  char *resultquery;
  resultquery = strtok(result->query, " ");

  //  TODO insert<fname><lname><section><birthdate>.Cette requête insère un nouvel étudiant dans la base de données en vérifiant que l’ID n’existe pas déjà.Si l’id existe déjà,l’insertion échoue.
  if (resultquery == "insert")
  {
    // student_t s = blabla
    new student_t s;
    // TODO ANDRIUS changer en  s->fname au lieu de fname etc
    s->fname = strtok(NULL, " ");
    lname = strtok(NULL, " ");
    section = strtok(NULL, " ");
    birthdate = strtok(NULL, " ");
    if (parse_insert(paramètres))
    {
      query_result_add(result, student_t s);
    }
    else
    {
      // indiquer au user que c'est faux/introduit de mauvais paramètres
    }
    // TODO update < filtre >= <valeur> set < champ_modifie >= <valeur_modifiee>.Cette requête modifie tous les étudiants correspondant au filtre < filtre >= <valeur>, en donnant la valeur<valeur_modifiee> au champ<champ_modifie>.
    if (resultquery == "update")
    {
    for
      student in result->students
      {
        if (student->filtre == valeur and parse_update(paramètres))
        {
          query_result_update(result, student_t s);
        }
      }

    // bool parse update
    }

    // TODO delete < champ >= <valeur>.Cette requête supprime tous les étudiants qui correspondent au filtre donné.
    if (resultquery == "delete")
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
    }

    // TODO select < champ >= <valeur>.Cette requête renvoie la liste des étudiants qui correspondent à l’unique filtre < champ >= <valeur>.
    if (resultquery == "select")
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
    }
    // TODO transaction.Ce mot - clef déclare le début d’une nouvelle transaction.Plus de détails sont donnés dans la Section
    if (resultquery == "transaction")
    {
      if (parse_transaction(paramètres))
      {
        query_result_transaction(result, student_t s);
      }
      else
      {
        // indiquer au user que c'est faux
      }
    }
  }

  void query_result_add(query_result_t * result, student_t s)
  {
  }
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

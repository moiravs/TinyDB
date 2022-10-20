#include "query.hpp"
#include "parsing.hpp"
#include <iostream>
#include <time.h>
#include <string.h>

void query_result_init(query_result_t *result, const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->psize = sizeof(student_t);
  result->lsize = 0;
  result->students = (student_t *)malloc(sizeof(student_t));
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  result->end_ns = after.tv_nsec + 1e9 * after.tv_sec;

}


void query_result_add(query_result_t *result, student_t s){
  query_list_upsize(result);
  result->students[result->lsize] = s;
  result->lsize += 1;
  result->status = QUERY_SUCCESS;
}

void query_list_upsize(query_result_t *result){

  if (result->lsize >= ((result->psize)/sizeof(student_t))){
    student_t *old_data = result->students;
    size_t old_psize = result->psize;
    result->psize *= 2;
    result->students = (student_t*)malloc(result->psize);
    memcpy(result->students, old_data, old_psize);
  }
}

void query_select_and_delete(database_t *db, query_result_t *queryResult, char *fieldFilter, char *value_str, char *value, student_t *s, char *date_str, const char *queryKey){
  for (size_t i = 0; i < db->lsize; i++)
    {
      *s = db->data[i];
      if (strcmp(fieldFilter, "id") == 0)
      {
        sprintf(value_str, "%u", s->id); // convertir le id (unsigned) à un char* pour la comparaison
        if (strcmp(value_str, value) == 0)
        {
          query_result_add(queryResult, *s);
          if (strcmp(queryKey, "delete")){
            db_delete(db, i);
          }
        }
      }
      else if (strcmp(fieldFilter, "fname") == 0)
      {
        if (strcmp(s->fname, value) == 0)
        {
          query_result_add(queryResult, *s);
          if (strcmp(queryKey, "delete")){
            db_delete(db, i);
          }
        }
      }
      else if (strcmp(fieldFilter, "lname") == 0)
      {
        if (strcmp(s->lname, value) == 0)
        {
          query_result_add(queryResult, *s);
          if (strcmp(queryKey, "delete")){
            db_delete(db, i);
          }
        }
      }
      else if (strcmp(fieldFilter, "section") == 0)
      {
        if (strcmp(s->section, value) == 0)
        {
          query_result_add(queryResult, *s);
          if (strcmp(queryKey, "delete")){
            db_delete(db, i);
          }
        }
      }
      else if (strcmp(fieldFilter, "birthdate") == 0)
      {
        strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
        if (strcmp(date_str, value) == 0)
        {
          query_result_add(queryResult, *s);
          if (strcmp(queryKey, "delete")){
            db_delete(db, i);
          }
        }
      }
      else {
        std::cout << "An error has occurred during the select query : bad filter." << std::endl;
        //break;
      }
    }
}
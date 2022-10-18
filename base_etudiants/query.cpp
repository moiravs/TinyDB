#include "query.hpp"
#include "parsing.hpp"
#include <iostream>
#include <time.h>
#include <string.h>

void query_result_init(query_result_t *result, const char *query)
{
  result->students = new student_t;
  result->psize = sizeof(student_t);
  char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
  sprintf(querymod, "%-255s", query);
  *result->query = *query;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  char *saveptr;
  const char * queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
  queryKey = strtok_r(querymod," ", &saveptr); 
  student_t *s = new student_t;
  char *fieldFilter = new char();
  char *valueFilter = new char();
  char *fieldToUpdate = new char();
  char *updateValue = new char();
  char *value = new char();
  if (strcmp(queryKey, "insert") == 0 && parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate)) // strcmp renvoie 0 si les strings sont les mêmes
  {
    // si l'id existe déjà, l'insertion échoue
    query_result_add(result, *s);
  }

  else if (strcmp(queryKey, "update") == 0 && parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue)){
    query_result_add(result, *s);
  }

  else if (strcmp(queryKey, "select") == 0 && parse_selectors(saveptr, fieldFilter, value)){
    
  }

  else if (strcmp(queryKey, "delete") == 0 && parse_selectors(saveptr, fieldFilter, value)){
    
  }

  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  result->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  result->status = QUERY_SUCCESS;

  delete queryKey;
}


void query_result_add(query_result_t *result, student_t s){
  result->lsize += 1;
  query_list_upsize(result);
  result->students[result->lsize+1] = s;
}

void query_list_upsize(query_result_t *result){
  if (result->lsize >= (result->psize/sizeof(student_t))){
    student_t *old_data = result->students;
    size_t old_psize = result->psize;
    result->psize *= 2;
    result->students = (student_t*)malloc(result->psize);
    memcpy(result->students, old_data, old_psize);
    free(old_data);
  }
}
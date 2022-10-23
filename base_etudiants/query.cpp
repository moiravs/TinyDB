#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
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

void query_result_add(query_result_t *result, student_t s)
{
  query_list_upsize(result);
  result->students[result->lsize] = s;
  result->lsize += 1;
  result->status = QUERY_SUCCESS;
}

void query_list_upsize(query_result_t *result)
{

  if (result->lsize >= ((result->psize) / sizeof(student_t)))
  {
    student_t *old_data = result->students;
    size_t old_psize = result->psize;
    result->psize *= 2;
    result->students = (student_t *)malloc(result->psize);
    memcpy(result->students, old_data, old_psize);
  }
}

void query_select_and_delete(database_t *db, query_result_t *queryResult, char *fieldFilter, char *value_str, char *value,char *date_str, const char *queryKey)
{
  student_t *s = new student_t;
  for (size_t i = 0; i < db->lsize; i++)
  {

    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(value_str, "%u", s->id); // convertir le id (unsigned) à un char* pour la comparaison
      if (strcmp(value_str, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
        {
          db_delete(db, i);
        }
      }
    }
    else if (strcmp(fieldFilter, "fname") == 0)
    {
      if (strcmp(s->fname, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
        {
          db_delete(db, i);
        }
      }
    }
    else if (strcmp(fieldFilter, "lname") == 0)
    {
      if (strcmp(s->lname, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
        {
          db_delete(db, i);
        }
      }
    }
    else if (strcmp(fieldFilter, "section") == 0)
    {
      if (strcmp(s->section, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
        {
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
        if (strcmp(queryKey, "delete"))
        {
          db_delete(db, i);
        }
      }
    }
    else
    {
      std::cout << "An error has occurred during the select query : bad filter." << std::endl;
      // break;
    }
  }
  delete s;
}
void query_update(database_t* db, char * fieldFilter, char* valueFilter, char* fieldToUpdate, char* updateValue){
   student_t *s = new student_t;
  for (size_t i = 0; i < db->lsize; i++)
  {

    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0 && strcmp(s->id,valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0){ s->id = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0) {*s->fname = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0){*s->lname = *updateValue;}
      else if (strcmp(fieldToUpdate, "section") == 0) {*s->section = *updateValue;}
      else if (strcmp(fieldToUpdate, "birthdate") == 0){*s->birthdate = *updateValue;}
    }
    else if (strcmp(fieldFilter, "fname") == 0 && strcmp(s->fname,valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0){ s->id = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0) {*s->fname = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0){*s->lname = *updateValue;}
      else if (strcmp(fieldToUpdate, "section") == 0) {*s->section = *updateValue;}
      else if (strcmp(fieldToUpdate, "birthdate") == 0){*s->birthdate = *updateValue;}
    }
    else if (strcmp(fieldFilter, "lname") == 0 && strcmp(s->lname, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0){ s->id = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0) {*s->fname = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0){*s->lname = *updateValue;}
      else if (strcmp(fieldToUpdate, "section") == 0) {*s->section = *updateValue;}
      else if (strcmp(fieldToUpdate, "birthdate") == 0){*s->birthdate = *updateValue;}
    }
    else if (strcmp(fieldFilter, "section") == 0 && strcmp(s->section, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0){ s->id = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0) {*s->fname = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0){*s->lname = *updateValue;}
      else if (strcmp(fieldToUpdate, "section") == 0) {*s->section = *updateValue;}
      else if (strcmp(fieldToUpdate, "birthdate") == 0){*s->birthdate = *updateValue;}
    }
    else if (strcmp(fieldFilter, "birthdate") == 0 && strcmp(s->fname, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0){ s->id = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0) {*s->fname = *updateValue;}
      else if (strcmp(fieldToUpdate, "lname") == 0){*s->lname = *updateValue;}
      else if (strcmp(fieldToUpdate, "section") == 0) {*s->section = *updateValue;}
      else if (strcmp(fieldToUpdate, "birthdate") == 0){*s->birthdate = *updateValue;}
    }
    else
    {
      std::cout << "An error has occurred during the select query : bad filter." << std::endl;
      // break;
    }
  }
  delete s;
}

void gestion_query(database_t *db, char *query, const char *keyWord)
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
  char value_str[64] = "0", date_str[64] = "0";

  
  queryResult->status = QUERY_SUCCESS;
  
  if (strcmp(keyWord, "insert") == 0)
  {
    student_t *s = new student_t;
    if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate))
    {
      db_add(db, *s);
    }
    else
    {
      std::cout << "An error has occurred during the insert query." << std::endl;
    }
  }
  else if (strcmp(keyWord, "update") == 0 && parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue))
  {
    query_update(db, fieldFilter, valueFilter, fieldToUpdate, updateValue)
  }
  else if (strcmp(keyWord, "select") == 0 && parse_selectors(saveptr, fieldFilter, value))
  {
    query_select_and_delete(db, queryResult, fieldFilter, value_str, value, date_str, queryKey);
  }

  else if (strcmp(keyWord, "delete") == 0 && parse_selectors(saveptr, fieldFilter, value))
  {
    query_select_and_delete(db, queryResult, fieldFilter, value_str, value, date_str, queryKey);
  }
  log_query(queryResult);
  /*
  if (queryResult->lsize > 0)
  {
    for (size_t i = 0; i < queryResult->lsize; i++)
    {
      char buffer[sizeof(student_t)] = "0";
      student_to_str(buffer, &queryResult->students[i]);
      std::cout << buffer;
    }
  }*/
  
  delete queryResult;
}
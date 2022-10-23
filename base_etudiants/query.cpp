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
  memcpy(result->query,query,256);
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

void query_insert(database_t *db, char *query, char *saveptr)
{
  query_result_t *queryResult = new query_result_t();
  query_result_init(queryResult, query);
  student_t *s = new student_t;
  if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate)){
    std::cout << "db " << &db << " db->data " << &db->data;
    db_add(db, *s);
    query_result_add(queryResult, *s);}
  else
    std::cout << "An error has occurred during the insert query." << std::endl;
  log_query(queryResult);
  char * buffer = new char[512];
  student_to_str(buffer, &db->data[db->lsize]);
  printf(buffer);
  delete queryResult;
}

void query_select_and_delete(database_t *db, char *query, char *saveptr, const char *queryKey)
{
  student_t *s = new student_t;
  query_result_t *queryResult = new query_result_t();
  query_result_init(queryResult, query);
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64](), *value = new char[64];
  char value_str[64] = "0", date_str[64] = "0";
  parse_selectors(saveptr, fieldFilter, value);
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
          db_delete(db, i);
      }
    }

    else if (strcmp(fieldFilter, "fname") == 0)
    {
      if (strcmp(s->fname, value) == 0)
      {
        query_result_add(queryResult, *s);
        puts("heeeee");
        if (strcmp(queryKey, "delete"))
          puts("yeeee");
          db_delete(db, i);
      }
    }
    else if (strcmp(fieldFilter, "lname") == 0)
    {
      if (strcmp(s->lname, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
          db_delete(db, i);
      }
    }
    else if (strcmp(fieldFilter, "section") == 0)
    {
      if (strcmp(s->section, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
          db_delete(db, i);
      }
    }
    else if (strcmp(fieldFilter, "birthdate") == 0)
    {
      strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
      if (strcmp(date_str, value) == 0)
      {
        query_result_add(queryResult, *s);
        if (strcmp(queryKey, "delete"))
          db_delete(db, i);
      }
    }
    else
    {
      std::cout << "An error has occurred during the select query : bad filter." << std::endl;
      // break;
    }
  }
  queryResult->status = QUERY_SUCCESS;
  for (size_t i = 0; i < db->lsize; i++)
  {
    char *buffer = new char[256];
    student_to_str(buffer, &db->data[i]);
    printf(buffer);
  }
  log_query(queryResult);
  delete s;
  delete queryResult;
}

void query_brol(student_t *s){
  
}


void query_update(database_t *db, char *saveptr, char *query)
{
  query_result_t *queryResult = new query_result_t();
  query_result_init(queryResult, query);
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64](), *value = new char[64];
  parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue);
  student_t *s = new student_t;
  for (size_t i = 0; i < db->lsize; i++)
  {

    *s = db->data[i];
    /*
    if (strcmp(fieldFilter, "id") == 0 && strcmp(s->id, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
        s->id = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->fname = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->lname = *updateValue;
      else if (strcmp(fieldToUpdate, "section") == 0)
        *s->section = *updateValue;
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
        *s->birthdate = *updateValue;
    }*/
    if (strcmp(fieldFilter, "fname") == 0 && strcmp(s->fname, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
        s->id = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->fname = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->lname = *updateValue;
      else if (strcmp(fieldToUpdate, "section") == 0)
        *s->section = *updateValue;
      else if (strcmp(fieldToUpdate, "birthdate") == 0);
       // *s->birthdate = *updateValue;
    }
    else if (strcmp(fieldFilter, "lname") == 0 && strcmp(s->lname, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
        s->id = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->fname = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->lname = *updateValue;
      else if (strcmp(fieldToUpdate, "section") == 0)
        *s->section = *updateValue;
      else if (strcmp(fieldToUpdate, "birthdate") == 0);
        //*s->birthdate = *updateValue;
    }
    else if (strcmp(fieldFilter, "section") == 0 && strcmp(s->section, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
        s->id = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->fname = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->lname = *updateValue;
      else if (strcmp(fieldToUpdate, "section") == 0)
        *s->section = *updateValue;
      else if (strcmp(fieldToUpdate, "birthdate") == 0);
        //*s->birthdate = *updateValue;
    }
    else if (strcmp(fieldFilter, "birthdate") == 0 && strcmp(s->fname, valueFilter))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
        s->id = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->fname = *updateValue;
      else if (strcmp(fieldToUpdate, "lname") == 0)
        *s->lname = *updateValue;
      else if (strcmp(fieldToUpdate, "section") == 0)
        *s->section = *updateValue;
      else if (strcmp(fieldToUpdate, "birthdate") == 0);
       // *s->birthdate = *updateValue;
    }
    else
      std::cout << "An error has occurred during the select query : bad filter." << std::endl;
      // break;
  }
  log_query(queryResult);
  delete s;
  delete queryResult;
}

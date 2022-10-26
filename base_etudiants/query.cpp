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
  memcpy(result->query, query, 256); // initialize query in result
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
  result->students[result->lsize] = s; // at end of list
  result->lsize += 1;
  result->status = QUERY_SUCCESS;
}

void query_list_upsize(query_result_t *result)
{

  if (result->lsize >= ((result->psize) / sizeof(student_t)))
  {
    student_t *old_data = result->students; // save old students list
    size_t old_psize = result->psize;
    result->psize *= 2;
    result->students = (student_t *)malloc(result->psize); // allocating new psize
    memcpy(result->students, old_data, old_psize);         // copy old data into new allocated memory
  }
}

void query_insert(database_t *db, query_result_t *queryResult, char *query, char *saveptr)
{
  student_t *s = new student_t;
  if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate))
  { // if valid insert query
    std::cout << "Adding " << s->fname << " " << s->lname << " to the database..." << std::endl;
    db_add(db, *s);
    query_result_add(queryResult, *s);
  }
  else
    std::cout << "An error has occurred during the insert query." << std::endl;
  log_query(queryResult);
}

void query_select_and_delete(database_t *db, query_result_t *queryResult, char *query, char *saveptr, const char *queryKey)
{
  student_t *s = new student_t;
  char *fieldFilter = new char[64](), *value = new char[64];
  char value_str[64] = "0", date_str[64] = "0";
  parse_selectors(saveptr, fieldFilter, value);
  if (strcmp(queryKey, "select") == 0)
  {
    std::cout << "Searching for all students whose " << fieldFilter << " is " << value << std::endl;
  }
  else if (strcmp(queryKey, "delete") == 0)
  {
    std::cout << "Deleting all students whose " << fieldFilter << " is " << value << std::endl;
  }
  for (size_t i = 0; i < db->lsize; i++) // iterating through database to find all students corresponding to the given filter
  {

    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(value_str, "%u", s->id); // convert id (unsigned) to char* for comparison
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
      std::cout << "An error has occurred during the select or delete query : bad filter." << std::endl;
    }
  }
  queryResult->status = QUERY_SUCCESS;
  log_query(queryResult);
  delete s;
  // delete queryResult;
}

void query_update(database_t *db, query_result_t *queryResult, char *saveptr, char *query)
{
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64];
  parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue); // check if valid query
  student_t *s = new student_t;
  for (size_t i = 0; i < db->lsize; i++)
  {
    *s = db->data[i];
    char *buffer = new char[512];
    student_to_str(buffer, s);
    printf("buffer : %s\n", buffer);
    char date_str[512] = "0";
    char id[64];
    printf("Update value : %s\n", updateValue);
    if (strcmp(fieldToUpdate, "id") == 0)
    {
      sprintf(id, "%u", s->id);
    }
    else if (strcmp(fieldToUpdate, "birthdate") == 0)
    {
      strftime(date_str, 512, "%d/%m/%Y", &s->birthdate); // store birthdate struct as a string
    }
    if ((strcmp(fieldFilter, "id") == 0) && (strcmp(id, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        *id = *updateValue;
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        memcpy(s->fname,updateValue,64);
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        memcpy(s->lname,updateValue,64);
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        *s->section = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        *date_str = *updateValue;
        strptime(date_str, "%d/%B/%Y", &s->birthdate); // transform the updated string to tm struct
        query_result_add(queryResult, *s);
      }
    }
    else if ((strcmp(fieldFilter, "fname") == 0) && (strcmp(s->fname, valueFilter) == 0))
    {
      std::cout << "je passe par là " << std::endl;
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        *id = *updateValue;
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        memcpy(s->fname,updateValue, 64);
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        memcpy(s->lname, updateValue,64);
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        *s->section = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        *date_str = *updateValue;
        strptime(date_str, "%d/%B/%Y", &s->birthdate);
        query_result_add(queryResult, *s);
      }
    }
    else if ((strcmp(fieldFilter, "lname") == 0) && (strcmp(s->lname, valueFilter) == 0))
    {
      puts("going through lname");
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        *id = *updateValue;
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        *s->fname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        *s->lname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        *s->section = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        *date_str = *updateValue;
        strptime(date_str, "%d/%B/%Y", &s->birthdate);
        query_result_add(queryResult, *s);
      }
    }
    else if ((strcmp(fieldFilter, "section") == 0) && (strcmp(s->section, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        *id = *updateValue;
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        *s->fname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        *s->lname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        *s->section = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        *date_str = *updateValue;
        strptime(date_str, "%d/%B/%Y", &s->birthdate);
        query_result_add(queryResult, *s);
      }
    }
    else if ((strcmp(fieldFilter, "birthdate") == 0) && (strcmp(date_str, valueFilter) == 0))
    {
      strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        *id = *updateValue;
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        *s->fname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        *s->lname = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        *s->section = *updateValue;
        query_result_add(queryResult, *s);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        *date_str = *updateValue;
        strptime(date_str, "%d/%B/%Y", &s->birthdate);
        query_result_add(queryResult, *s);
      }
    }
    else
      std::cout << "An error has occurred during the update query : bad filter." << std::endl;
    // break;
  }
  log_query(queryResult);
  delete s;
  // delete queryResult;
}

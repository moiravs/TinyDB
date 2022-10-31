/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include <iostream>
#include <time.h>
#include <string.h>
#include <sys/mman.h>

void query_result_t::query_result_add(student_t s)
{
  this->query_list_upsize();
  this->students[this->lsize] = s; // at end of list
  this->lsize += 1;
  this->status = QUERY_SUCCESS;
}

void query_result_t::query_list_upsize()
{

  if (this->lsize >= ((this->psize) / sizeof(student_t)))
  {
    student_t *old_data = this->students; // save old students list
    size_t old_psize = this->psize;
    this->psize *= 2;
    this->students = (student_t *)malloc(this->psize); // allocating new psize
    memcpy(this->students, old_data, old_psize);         // copy old data into new allocated memory
  }
}

void query_result_t::query_insert(database_t *db, char *query, char *saveptr)
{
  student_t *s = new student_t;
  if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate))
  { // if valid insert query
    std::cout << "Adding " << s->fname << " " << s->lname << " to the database..." << std::endl;
    db_add(db, *s);
    this->query_result_add(*s);
  }
  else
    std::cout << "An error has occurred during the insert query." << std::endl;
  this->status = QUERY_SUCCESS;
  log_query(this);
  delete s;
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
  size_t i = 0;
  while (i < db->lsize) // iterating through database to find all students corresponding to the given filter
  {
    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(value_str, "%u", s->id); // convert id (unsigned) to char* for comparison
      if (strcmp(value_str, value) == 0)
      {
        queryResult->query_result_add(*s);
        ;
        if (strcmp(queryKey, "delete"))
        {
          db_delete(db, i);
          i--;
        }
      }
    }

    else if (strcmp(fieldFilter, "fname") == 0)
    {
      if (strcmp(s->fname, value) == 0)
      {
        queryResult->query_result_add(*s);
        ;
        std::cout << "i found" << std::endl;
        if (strcmp(queryKey, "delete") == 0)
        {
          std::cout << "weird";
          db_delete(db, i);
          i--;
        }
      }
    }
    else if (strcmp(fieldFilter, "lname") == 0)
    {
      if (strcmp(s->lname, value) == 0)
      {
        queryResult->query_result_add(*s);
        ;
        if (strcmp(queryKey, "delete") == 0)
        {
          db_delete(db, i);
          i--;
        }
      }
    }
    else if (strcmp(fieldFilter, "section") == 0)
    {
      if (strcmp(s->section, value) == 0)
      {
        queryResult->query_result_add(*s);
        ;
        if (strcmp(queryKey, "delete") == 0)
        {
          db_delete(db, i);
          i--;
        }
      }
    }
    else if (strcmp(fieldFilter, "birthdate") == 0)
    {
      strftime(date_str, 44, "%d/%m/%Y", &s->birthdate);
      std::cout << date_str << std::endl;
      if (strcmp(date_str, value) == 0)
      {
        queryResult->query_result_add(*s);
        ;
        if (strcmp(queryKey, "delete") == 0)
        {
          db_delete(db, i);
          i--;
        }
      }
    }
    else
    {
      // std::cout << "An error has occurred during the select or delete query : bad filter." << std::endl;
    }
    i++;
  }
  queryResult->status = QUERY_SUCCESS;
  msync(&db, db->psize, 2);
  log_query(queryResult);
  delete s;
}

void query_update(database_t *db, query_result_t *queryResult, char *saveptr, char *query)
{
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64];
  parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue); // check if valid query
  student_t *s = new student_t;

  for (size_t i = 0; i < db->lsize; i++)
  {
    *s = db->data[i];
    /*
    char *buffer = new char[512];
    student_to_str(buffer, s);
    printf("buffer : %s\n", buffer);*/
    char date_str[512] = "0";
    char id[64];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(id, "%u", s->id);
    }
    else if (strcmp(fieldToUpdate, "birthdate") == 0)
    {
      strftime(date_str, 512, "%d/%m/%Y", &s->birthdate); // store birthdate struct as a string
    }
    if ((strcmp(fieldFilter, "id") == 0) && (strcmp(id, valueFilter) == 0))
    {
      puts("hii");
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(db->data[i]); // transform the updated string to tm struct
      }
    }
    else if ((strcmp(fieldFilter, "fname") == 0) && (strcmp(s->fname, valueFilter) == 0))
    {
      std::cout << "je passe par là " << std::endl;
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);                          // copy id in the variable updateValue
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        queryResult->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "lname") == 0) && (strcmp(s->lname, valueFilter) == 0))
    {
      puts("going through lname");
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(*s);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        queryResult->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "section") == 0) && (strcmp(s->section, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        queryResult->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "birthdate") == 0) && (strcmp(date_str, valueFilter) == 0))
    {
      strftime(date_str, 32, "%d/%m/%Y", &s->birthdate);
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                    // conversion to long int
        s->id = static_cast<unsigned int>(temp); // conversion to unsigned
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        queryResult->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        queryResult->query_result_add(db->data[i]);
      }
    }
  }
  queryResult->status = QUERY_SUCCESS;
  msync(&db, db->psize, 2);
  log_query(queryResult);
  delete s;
}

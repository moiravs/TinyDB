/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include <iostream>
#include <time.h>

query_result_t::query_result_t(const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  this->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  memcpy(this->query, query, 256); // initialize query in result
  this->psize = sizeof(student_t) * 16;
  this->lsize = 0;
  this->students = (student_t *)malloc(this->psize);
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
}
void query_result_t::log_query()
{
  char buffer[512];
  if (this->status == QUERY_SUCCESS)
  {
    char filename[512];
    char type[256];
    strcpy(type, this->query);
    type[6] = '\0';
    sprintf(filename, "logs/%ld-%s.txt", this->start_ns, type);
    printf("%s\n", filename);
    FILE *f = fopen(filename, "w");
    float duration = (float)(this->end_ns - this->start_ns) / 1.0e6;
    sprintf(buffer, "Query \"%s\" completed in %fms with %ld results.\n", this->query, duration, this->lsize);
    fwrite(buffer, sizeof(char), strlen(buffer), f);

    if (this->lsize > 0)
    {
      for (size_t i = 0; i < this->lsize; i++)
      {
        this->students[i].student_to_str(buffer);
        fwrite(buffer, sizeof(char), strlen(buffer), f);
        fwrite("\n", sizeof(char), 1, f);
      }
    }
    fclose(f);
  }
  else if (this->status == UNRECOGNISED_FIELD)
  {
    fprintf(stderr, "Unrecognized field in query %s\n", this->query);
  }
}

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
    memcpy(this->students, old_data, old_psize);       // copy old data into new allocated memory
  }
}

void query_result_t::query_insert(database_t *db, char *query, char *saveptr)
{
  student_t *s = new student_t;
  if (parse_insert(saveptr, s->fname, s->lname, &s->id, s->section, &s->birthdate))
  { // if valid insert query
    std::cout << "Adding " << s->fname << " " << s->lname << " to the database..." << std::endl;
    db->db_add(*s);
    this->query_result_add(*s);
  }
  else
    std::cout << "Invalid Arguments : insert <fname> <lname> <id> <section> <birthdate>" << std::endl;
  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
  delete s;
}

void query_result_t::query_select(database_t *db, char *query, char *saveptr)
{
  student_t *s = new student_t;
  this->status = QUERY_FAILURE;
  char *fieldFilter = new char[64](), *value = new char[64];
  char value_str[64] = "0", date_str[64] = "0";
  if (!parse_selectors(saveptr, fieldFilter, value))
  {
    puts("Invalid Arguments : select <champ>=<valeur>");
    return;
  }
  std::cout << "Searching for all students whose " << fieldFilter << " is " << value << std::endl;
  size_t i = 0;
  while (i < db->lsize) // iterating through database to find all students corresponding to the given filter
  {
    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(value_str, "%u", s->id); // convert id (unsigned) to char* for comparison
      if (strcmp(value_str, value) == 0)
      {
        this->query_result_add(*s);
      }
    }

    else if (strcmp(fieldFilter, "fname") == 0)
    {
      if (strcmp(s->fname, value) == 0)
      {
        this->query_result_add(*s);
      }
    }
    else if (strcmp(fieldFilter, "lname") == 0)
    {
      if (strcmp(s->lname, value) == 0)
      {
        this->query_result_add(*s);
      }
    }
    else if (strcmp(fieldFilter, "section") == 0)
    {
      if (strcmp(s->section, value) == 0)
      {
        this->query_result_add(*s);
      }
    }
    else if (strcmp(fieldFilter, "birthdate") == 0)
    {
      strftime(date_str, 44, "%d/%m/%Y", &s->birthdate);
      if (strcmp(date_str, value) == 0)
      {
        this->query_result_add(*s);
      }
    }
    else
    {
      std::cout << "An error has occurred during the select query : bad filter." << std::endl;
      this->status = UNRECOGNISED_FIELD;
      return;
    }
    i++;
  }

  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
  delete s;
}

void query_result_t::query_delete(database_t *db, char *query, char *saveptr)
{
  student_t *s = new student_t;
  this->status = QUERY_FAILURE;
  char *fieldFilter = new char[64](), *value = new char[64];
  char value_str[64] = "0", date_str[64] = "0";
  if (!parse_selectors(saveptr, fieldFilter, value))
  {
    puts("Invalid Arguments : delete <champ>=<valeur>");
    return;
  }
  std::cout << "Deleting all students whose " << fieldFilter << " is " << value << std::endl;
  size_t i = 0;
  while (i < db->lsize) // iterating through database to find all students corresponding to the given filter
  {
    *s = db->data[i];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(value_str, "%u", s->id); // convert id (unsigned) to char* for comparison
      if (strcmp(value_str, value) == 0)
      {
        this->query_result_add(*s);
        db->db_delete(i);
        i--;
      }
    }

    else if (strcmp(fieldFilter, "fname") == 0)
    {
      if (strcmp(s->fname, value) == 0)
      {
        this->query_result_add(*s);
        db->db_delete(i);
        i--;
      }
    }
    else if (strcmp(fieldFilter, "lname") == 0)
    {
      if (strcmp(s->lname, value) == 0)
      {
        this->query_result_add(*s);
        db->db_delete(i);
        i--;
      }
    }
    else if (strcmp(fieldFilter, "section") == 0)
    {
      if (strcmp(s->section, value) == 0)
      {
        this->query_result_add(*s);
        db->db_delete(i);
        i--;
      }
    }
    else if (strcmp(fieldFilter, "birthdate") == 0)
    {
      strftime(date_str, 44, "%d/%m/%Y", &s->birthdate);
      if (strcmp(date_str, value) == 0)
      {
        this->query_result_add(*s);
        db->db_delete(i);
        i--;
      }
    }
    else
    {
      std::cout << "An error has occurred during the delete query : bad filter." << std::endl;
      this->status = UNRECOGNISED_FIELD;
      return;
    }
    i++;
  }

  this->status = QUERY_SUCCESS;

  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
  delete s;
}

void query_result_t::query_update(database_t *db, char *query, char *saveptr)
{
  char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64];
  if (!parse_update(saveptr, fieldFilter, valueFilter, fieldToUpdate, updateValue))
  {
    puts("Invalid Arguments : update <filtre>=<valeur> set <champ_modifie>=<valeur_modifiee>"); // check if valid query
    return;
  }

  for (size_t i = 0; i < db->lsize; i++)
  {
    char date_str[512] = "0";
    char id[64];
    if (strcmp(fieldFilter, "id") == 0)
    {
      sprintf(id, "%u", db->data[i].id);
    }
    else if (strcmp(fieldToUpdate, "birthdate") == 0)
    {
      strftime(date_str, 512, "%d/%m/%Y", &db->data[i].birthdate); // store birthdate struct as a string
    }
    if ((strcmp(fieldFilter, "id") == 0) && (strcmp(id, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);              // transform the updated string to tm struct
      }
    }
    else if ((strcmp(fieldFilter, "fname") == 0) && (strcmp(db->data[i].fname, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);                          // copy id in the variable updateValue
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        this->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "lname") == 0) && (strcmp(db->data[i].lname, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        this->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "section") == 0) && (strcmp(db->data[i].section, valueFilter) == 0))
    {
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        this->query_result_add(db->data[i]);
      }
    }
    else if ((strcmp(fieldFilter, "birthdate") == 0) && (strcmp(date_str, valueFilter) == 0))
    {
      strftime(date_str, 32, "%d/%m/%Y", &db->data[i].birthdate);
      if (strcmp(fieldToUpdate, "id") == 0)
      {
        strcpy(id, updateValue);
        long temp = atol(id);                             // conversion to long int
        db->data[i].id = static_cast<unsigned int>(temp); // conversion to unsigned
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "fname") == 0)
      {
        strcpy(db->data[i].fname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "lname") == 0)
      {
        strcpy(db->data[i].lname, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "section") == 0)
      {
        strcpy(db->data[i].section, updateValue);
        this->query_result_add(db->data[i]);
      }
      else if (strcmp(fieldToUpdate, "birthdate") == 0)
      {
        strcpy(date_str, updateValue);
        strptime(date_str, "%d/%m/%Y", &db->data[i].birthdate);
        this->query_result_add(db->data[i]);
      }
    }
  }
  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
}

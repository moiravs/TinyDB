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
#include <string.h>

query_result_t::query_result_t(const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  this->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  memcpy(this->query, query, 256); // initialize query in result
  this->psize = sizeof(student_t) * 16;
  this->lsize = 0;
  this->students = (student_t *)malloc(this->psize);
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
        student_to_str(&this->students[i], buffer);
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

void query_result_t::query_insert(database_t *db, char *query, char *p_end_of_query)
{
  student_t *s = new student_t;
  if (!parse_insert(p_end_of_query, s->fname, s->lname, &s->id, s->section, &s->birthdate)) // if not valid insert query
  {
    std::cout << "Invalid Arguments : insert <fname> <lname> <id> <section> <birthdate>" << std::endl;
    return;
  }
  std::cout << "Adding " << s->fname << " " << s->lname << " to the database..." << std::endl;
  db->db_add(*s);
  this->query_result_add(*s);
  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
  delete s;
}

void query_result_t::query_select(database_t *db, char *query, char *p_end_of_query)
{
  this->status = QUERY_FAILURE;
  char *field_filter = new char[64](), *value = new char[64];
  if (!parse_selectors(p_end_of_query, field_filter, value))
  {
    puts("Invalid Arguments : select <champ>=<valeur>");
    return;
  }
  std::cout << "Searching for all students whose " << field_filter << " is " << value << std::endl;
  size_t i = 0;
  while (i < db->get_lsize()) // iterating through database to find all students corresponding to the given filter
  {
    student_t *record = db->get_record(i);
    if (is_student_ok(record, field_filter, value))
      this->query_result_add(*record);
    i++;
  }

  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
}

bool query_result_t::is_student_ok(student_t *s, char *field_filter, char *value)
{
  char value_str[64] = "0", date_str_one[64] = "0", date_str_two[64] = "0";
  if (strcmp(field_filter, "id") == 0)
  {
    sprintf(value_str, "%u", s->id); // convert id (unsigned) to char* for comparison
    if (strcmp(value_str, value) == 0)
      return true;
  }
  else if (strcmp(field_filter, "fname") == 0)
  {
    if (strcmp(s->fname, value) == 0)
      return true;
  }
  else if (strcmp(field_filter, "lname") == 0)
  {
    if (strcmp(s->lname, value) == 0)
      return true;
  }
  else if (strcmp(field_filter, "section") == 0)
  {
    if (strcmp(s->section, value) == 0)
      return true;
  }
  else if (strcmp(field_filter, "birthdate") == 0)
  {
    strftime(date_str_one, 44, "%d/%m/%Y", &s->birthdate);                                                              // accept format with leading zeros
    snprintf(date_str_two, 44, "%d/%d/%d", s->birthdate.tm_mday, s->birthdate.tm_mon + 1, s->birthdate.tm_year + 1900); // accept date format with non-leading zeros
    if ((strcmp(date_str_one, value) == 0) || (strcmp(date_str_two, value) == 0))
      return true;
  }
  else
  {
    std::cout << "An error has occurred during the query : bad filter." << std::endl;
    this->status = UNRECOGNISED_FIELD;
    this->log_query();
    return false;
  }
  return false;
}

void query_result_t::query_delete(database_t *db, char *query, char *p_end_of_query)
{
  this->status = QUERY_FAILURE;
  char *field_filter = new char[64](), *value = new char[64];

  if (!parse_selectors(p_end_of_query, field_filter, value))
  {
    puts("Invalid Arguments : delete <champ>=<valeur>");
    return;
  }
  std::cout << "Deleting all students whose " << field_filter << " is " << value << std::endl;
  size_t i = 0;
  while (i < db->get_lsize()) // iterating through database to find all students corresponding to the given filter
  {
    student_t *record = db->get_record(i);
    if (is_student_ok(record, field_filter, value))
    {
      this->query_result_add(*record);
      db->db_delete(i);
      i--;
    }
    i++;
  }
  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
}

void query_result_t::query_update(database_t *db, char *query, char *p_end_of_query)
{
  char *field_filter = new char[64](), *value_filter = new char[64](), *field_to_update = new char[64](), *update_value = new char[64];
  if (!parse_update(p_end_of_query, field_filter, value_filter, field_to_update, update_value))
  {
    puts("Invalid Arguments : update <filtre>=<valeur> set <champ_modifie>=<valeur_modifiee>"); // check if valid query
    return;
  }
  for (size_t i = 0; i < db->get_lsize(); i++)
  {
    student_t *record = db->get_record(i);

    if (is_student_ok(record, field_filter, value_filter))
    {
      if (strcmp(field_to_update, "id") == 0)
      {
        student_t *s = new student_t;
        memcpy(s->fname, &record->fname, 64);          // copie l'étudiant record
        memcpy(s->section, &record->section, 64);      // copie l'étudiant record
        memcpy(s->lname, &record->lname, 64);          // copie l'étudiant record
        memcpy(&s->birthdate, &record->birthdate, 44); // copie l'étudiant record
        int temp = (atoi(update_value));
        memcpy(&s->id, &temp, 4); // change l'id du nouvel étudiant créé
        db->db_delete(i);
        if (db->db_add(*s)) // si l'id n'est pas présent
        {
          this->query_result_add(*s);
          puts("Two students can't have the same ID, update query stops here");
          break;
        }
        else // si l'id est déjà dans la database
        {
          db->db_add(*record); // on remet l'ancien student
          puts("ID already in the database, update query stops here");
          break;
        }
      }
      else if (strcmp(field_to_update, "fname") == 0)
      {
        strcpy(record->fname, update_value);
        this->query_result_add(*record);
      }
      else if (strcmp(field_to_update, "lname") == 0)
      {
        strcpy(record->lname, update_value);
        this->query_result_add(*record);
      }
      else if (strcmp(field_to_update, "section") == 0)
      {
        strcpy(record->section, update_value);
        this->query_result_add(*record);
      }
      else if (strcmp(field_to_update, "birthdate") == 0)
      {
        strptime(update_value, "%d/%m/%Y", &record->birthdate);
        this->query_result_add(*record); // transform the updated string to tm struct
      }
      else
      {
        puts("The field to update in the update query doesn't exist. Choose between id, fname, lname, section and birthdate");
        this->status = UNRECOGNISED_FIELD;
        this->log_query();
        return;
      }
    }
  }
  this->status = QUERY_SUCCESS;
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  this->end_ns = after.tv_nsec + 1e9 * after.tv_sec;
  this->log_query();
}

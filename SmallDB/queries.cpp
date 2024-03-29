/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#include "queries.hpp"
#include <string>
#include <iostream>
#include <mutex>
#include <iterator>

// execute_* ///////////////////////////////////////////////////////////////////
std::mutex new_access;
std::mutex write_access;
std::mutex reader_registration;
int readers_c = 0;

void execute_select(FILE *fout, database_t *const db, const char *const field,
                    const char *const value)
{
  std::function<bool(const student_t &)> predicate = get_filter(field, value);
  if (!predicate)
  {
    query_fail_bad_filter(fout, field, value);
    return;
  }
  char *student = new char[256];
  int number_student = 0;
  new_access.lock();
  reader_registration.lock();
  if (readers_c == 0)
    write_access.lock();
  readers_c++;
  new_access.unlock();
  reader_registration.unlock();
  for (const student_t &s : db->data)
  {
    if (predicate(s))
    {
      number_student++;
      student_to_str(student, &s, 256);
      fputs(student, fout);
      fputs("\n", fout);
    }
  }
  reader_registration.lock();
  readers_c--;
  if (readers_c == 0)
    write_access.unlock();
  reader_registration.unlock();
  fprintf(fout, "%d", number_student);
  fprintf(fout, "%s", " student(s) selected\n");
  delete[] (student);
}

void execute_update(FILE *fout, database_t *const db, const char *const ffield, const char *const fvalue, const char *const efield, const char *const evalue)
{
  std::function<bool(const student_t &)> predicate = get_filter(ffield, fvalue);
  if (!predicate)
  {
    query_fail_bad_filter(fout, ffield, fvalue);
    return;
  }
  std::function<void(student_t &)> updater = get_updater(efield, evalue);
  if (!updater)
  {
    query_fail_bad_update(fout, efield, evalue);
    return;
  }
  int number_student = 0;
  new_access.lock();
  reader_registration.lock();
  if (readers_c == 0)
    write_access.lock();
  readers_c++;
  new_access.unlock();
  reader_registration.unlock();
  for (student_t &s : db->data)
  {
    if (predicate(s))
    {
      updater(s);
      number_student++;
    }
  }
  reader_registration.lock();
  readers_c--;
  if (readers_c == 0)
    write_access.unlock();
  reader_registration.unlock();
  fprintf(fout, "%d", number_student);
  fprintf(fout, "%s", " student(s) updated\n");
  fputs("\0", fout);
}

bool is_id_existing(database_t *const db, const unsigned id)
{
  std::function<bool(const student_t &)> predicate = get_filter("id", std::to_string(id).c_str());
  new_access.lock();
  reader_registration.lock();
  if (readers_c == 0)
    write_access.lock();
  readers_c++;
  new_access.unlock();
  reader_registration.unlock();
  for (const student_t &s : db->data)
  {
    if (predicate(s))
      return true;
  }
  reader_registration.lock();
  readers_c--;
  if (readers_c == 0)
    write_access.unlock();
  reader_registration.unlock();
  return false;
}

void execute_insert(FILE *fout, database_t *const db, const char *const fname,
                    const char *const lname, const unsigned id, const char *const section,
                    const tm birthdate)
{
  if (is_id_existing(db, id))
  {
    fputs("ID already in the database\n", fout);
    return;
  }
  new_access.lock();
  write_access.lock();
  new_access.unlock();
  db->data.emplace_back();
  student_t *s = &db->data.back();
  s->id = id;
  snprintf(s->fname, sizeof(s->fname), "%s", fname);
  snprintf(s->lname, sizeof(s->lname), "%s", lname);
  snprintf(s->section, sizeof(s->section), "%s", section);
  s->birthdate = birthdate;
  write_access.unlock();
  char *student = new char[256];
  student_to_str(student, s, 256);
  fputs(student, fout);
  fputs("\n", fout);
  delete[] (student);
}

void execute_delete(FILE *fout, database_t *const db, const char *const field,
                    const char *const value)
{
  std::function<bool(const student_t &)> predicate = get_filter(field, value);
  if (!predicate)
  {
    query_fail_bad_filter(fout, field, value);
    return;
  }
  new_access.lock();
  reader_registration.lock();
  if (readers_c == 0)
    write_access.lock();
  readers_c++;
  new_access.unlock();
  reader_registration.unlock();
  auto new_end = remove_if(db->data.begin(), db->data.end(), predicate);
  int number_student = db->data.end() - new_end;
  fprintf(fout, "%d", number_student);
  fprintf(fout, "%s", " student(s) deleted\n");
  fputs("\0", fout);
  db->data.erase(new_end, db->data.end());
  reader_registration.lock();
  readers_c--;
  if (readers_c == 0)
    write_access.unlock();
  reader_registration.unlock();
}

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select(FILE *fout, database_t *db, const char *const query)
{
  char ffield[32], fvalue[64]; // filter data
  int counter;
  if (sscanf(query, "select %31[^=]=%63s%n", ffield, fvalue, &counter) != 2)
    query_fail_bad_format(fout, "select");
  else if (static_cast<unsigned>(counter) < strlen(query))
  {
    std::cout << strlen(query) << " c " << counter << std::endl;
    query_fail_too_long(fout, "select");
  }
  else
    execute_select(fout, db, ffield, fvalue);
}

void parse_and_execute_update(FILE *fout, database_t *db, const char *const query)
{
  char ffield[32], fvalue[64]; // filter data
  char efield[32], evalue[64]; // edit data
  int counter;
  if (sscanf(query, "update %31[^=]=%63s set %31[^=]=%63s%n", ffield, fvalue, efield, evalue,
             &counter) != 4)
    query_fail_bad_format(fout, "update");
  else if (static_cast<unsigned>(counter) < strlen(query))
    query_fail_too_long(fout, "update");
  else
    execute_update(fout, db, ffield, fvalue, efield, evalue);
}

void parse_and_execute_insert(FILE *fout, database_t *db, const char *const query)
{
  char fname[64], lname[64], section[64], date[11];
  unsigned id;
  tm birthdate;
  int counter;
  if (sscanf(query, "insert %63s %63s %u %63s %10s%n", fname, lname, &id, section, date, &counter) != 5 || strptime(date, "%d/%m/%Y", &birthdate) == NULL)
    query_fail_bad_format(fout, "insert");
  else if (static_cast<unsigned>(counter) < strlen(query))
    query_fail_too_long(fout, "insert");
  else
    execute_insert(fout, db, fname, lname, id, section, birthdate);
}

void parse_and_execute_delete(FILE *fout, database_t *db, const char *const query)
{
  char ffield[32], fvalue[64]; // filter data
  int counter;
  if (sscanf(query, "delete %31[^=]=%63s%n", ffield, fvalue, &counter) != 2)
    query_fail_bad_format(fout, "delete");
  else if (static_cast<unsigned>(counter) < strlen(query))
    query_fail_too_long(fout, "delete");
  else
    execute_delete(fout, db, ffield, fvalue);
}

void parse_and_execute(FILE *fout, database_t *db, const char *const query)
{
  if (strncmp("select", query, sizeof("select") - 1) == 0)
    parse_and_execute_select(fout, db, query);
  else if (strncmp("update", query, sizeof("update") - 1) == 0)
    parse_and_execute_update(fout, db, query);
  else if (strncmp("insert", query, sizeof("insert") - 1) == 0)
    parse_and_execute_insert(fout, db, query);
  else if (strncmp("delete", query, sizeof("delete") - 1) == 0)
    parse_and_execute_delete(fout, db, query);
  else
    query_fail_bad_query_type(fout);
}

// query_fail_* ///////////////////////////////////////////////////////////////

void query_fail_bad_query_type(FILE *const fout)
{
  fprintf(fout, "Error : Unknown Query Type\n");
}

void query_fail_bad_format(FILE *const fout, const char *const query_type)
{
  std::string a = "Error : Bad Format in " + static_cast<std::string>(query_type) + "\n";
  fprintf(fout, "%s", a.c_str());
}

void query_fail_too_long(FILE *const fout, const char *const query_type)
{
  std::string a = "Error : Query Too long in" + static_cast<std::string>(query_type) + "\n";
  fprintf(fout, "%s", a.c_str());
}

void query_fail_bad_filter(FILE *const fout, const char *const field, const char *const filter)
{
  std::string a = "Error : Bad Field or Filter in " + static_cast<std::string>(field) + "=" + static_cast<std::string>(filter) + "\n";
  fprintf(fout, "%s", a.c_str());
}

void query_fail_bad_update(FILE *const fout, const char *const field, const char *const filter)
{
  std::string a = "Error : Bad Field of Filter in update, " + static_cast<std::string>(field) + "=" + static_cast<std::string>(filter) + " is not valid\n";
  fprintf(fout, "%s", a.c_str());
}

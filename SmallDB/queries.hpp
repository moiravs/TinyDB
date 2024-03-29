/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#ifndef _QUERIES_HPP
#define _QUERIES_HPP

#include <cstdio>

#include "db.hpp"

/**
 * @brief  Check if id is already in the database or not
 * @param  db: database
 * @param  id: id of the student
 * @retval True if id is in the database, false otherwise
 */
bool is_id_existing(database_t *const db, const unsigned id);

// execute_* //////////////////////////////////////////////////////////////////

void execute_select(FILE *fout, database_t *const db, const char *const field,
                    const char *const value);

void execute_update(FILE *fout, database_t *const db, const char *const ffield,
                    const char *const fvalue, const char *const efield, const char *const evalue);

void execute_insert(FILE *fout, database_t *const db, const char *const fname,
                    const char *const lname, const unsigned id, const char *const section,
                    const tm birthdate);

void execute_dump(FILE *fout, database_t *const db);

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select(FILE *fout, database_t *db, const char *const query);

void parse_and_execute_update(FILE *fout, database_t *db, const char *const query);

void parse_and_execute_insert(FILE *fout, database_t *db, const char *const query);

void parse_and_execute_delete(FILE *fout, database_t *db, const char *const query);

void parse_and_execute(FILE *fout, database_t *db, const char *const query);

// query_fail_* ///////////////////////////////////////////////////////////////

/** Those methods write a descriptive error message on fout */

void query_fail_bad_query_type(FILE *const fout);

void query_fail_bad_format(FILE *const fout, const char *const query_type);

void query_fail_too_long(FILE *const fout, const char *const query_type);

void query_fail_bad_filter(FILE *const fout, const char *const field, const char *const filter);

void query_fail_bad_update(FILE *const fout, const char *const field, const char *const filter);

#endif

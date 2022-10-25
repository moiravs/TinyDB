#ifndef _QUERY_H
#define _QUERY_H

#include "db.hpp"

typedef enum
{
  QUERY_SUCCESS,
  QUERY_FAILURE,
  UNRECOGNISED_FIELD
} QUERY_STATUS;

/**
 * A query_result_t describes the result of a query.
 */
typedef struct
{
  student_t *students;           /** A list of students **/
  size_t lsize;                  /** Logical size **/
  size_t psize;                  /** Physical size **/
  QUERY_STATUS status;           /** The return status of the query **/
  char query[sizeof(student_t)]; /** The actual query that was submitted **/
  long start_ns;                 /** The start of the query in nanoseconds **/
  long end_ns;                   /** The end of the query in nanoseconds **/
} query_result_t;

/**
 * Initialise a query_result_t structure.
 **/
void query_result_init(query_result_t *result, const char *query);

/**
 * Add a student to a query result.
 **/
void query_result_add(query_result_t *result, student_t s);

/**
 * Makes the list containing the students of a query bigger
 **/
void query_list_upsize(query_result_t *result);

/**
 * Handles the insert query
 **/
void query_insert(database_t *db, query_result_t * queryResult, char *query, char *saveptr);

/**
 * Handles the update query
 */
void query_update(database_t *db, query_result_t *queryResult, char *saveptr, char *query);

/**
  * Handles the select query
  **/
void query_select_and_delete(database_t *db, query_result_t *queryResult, char *query, char *saveptr, const char *queryKey);

#endif
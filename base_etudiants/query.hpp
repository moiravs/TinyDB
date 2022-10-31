#ifndef _QUERY_H
#define _QUERY_H

#include "db.hpp"
#include <cstdlib>

typedef enum
{
  QUERY_SUCCESS,
  QUERY_FAILURE,
  UNRECOGNISED_FIELD
} QUERY_STATUS;

/**
 * A query_result_t describes the result of a query.
 */
class query_result_t
{
  public : 
    student_t *students;           /** A list of students **/
    size_t lsize;                  /** Logical size **/
    size_t psize;                  /** Physical size **/
    QUERY_STATUS status;           /** The return status of the query **/
    char query[sizeof(student_t)]; /** The actual query that was submitted **/
    long start_ns;                 /** The start of the query in nanoseconds **/
    long end_ns;                   /** The end of the query in nanoseconds **/
    /**
     * Initialise a query_result_t structure.
     **/
    query_result_t(const char* query){
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
    /**
     * Add a student to a query result.
     **/
    void query_result_add(student_t s);
    /**
     * Makes the list containing the students of a query bigger
     **/
    void query_list_upsize();
    /**
     * Handles the insert query
     **/
    void query_insert(database_t *db, char *query, char *saveptr);
} ;


/**
 * Handles the update query
 */
void query_update(database_t *db, query_result_t *queryResult, char *saveptr, char *query);

/**
  * Handles the select query
  **/
void query_select_and_delete(database_t *db, query_result_t *queryResult, char *query, char *saveptr, const char *queryKey);

#endif
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

  public:
    /**
     * Constructor - Initialise a query_result_t structure.
     **/
    query_result_t(const char* query);
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
    /**
     * Handles the update query
     */
    void query_update(database_t *db, char *saveptr, char *query);

    /**
     * Handles the select query
     **/
    void query_select_and_delete(database_t *db, char *query, char *saveptr, const char *queryKey);
} ;




#endif
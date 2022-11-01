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
class query_result_t
{
public:
  student_t *students;           /** A list of students **/
  size_t lsize;                  /** Logical size **/
  size_t psize;                  /** Physical size **/
  QUERY_STATUS status;           /** The return status of the query **/
  char query[sizeof(student_t)]; /** The actual query that was submitted **/
  long start_ns;                 /** The start of the query in nanoseconds **/
  long end_ns;                   /** The end of the query in nanoseconds **/

public:
  /**
   * @brief  Constructor - Initialise a query_result_t structure.
   * @note
   * @param  *query: query that was submitted
   */
  query_result_t(const char *query);

  /**
   * @brief  Add a student to a query result.
   * @param  s: student to add
   * @retval None
   */
  void query_result_add(student_t s);
  /**
   * @brief  Makes the list containing the students of a query bigger
   * @retval None
   */
  void query_list_upsize();
  /**
   * @brief   Handles the insert query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *saveptr: Pointer to the rest of the query
   * @retval None
   */
  void query_insert(database_t *db, char *query, char *saveptr);
  /**
   * @brief   Handles the update query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *saveptr: Pointer to the rest of the query
   * @retval None
   */
  void query_update(database_t *db, char *query, char *saveptr);
  /**
   * @brief   Handles the select query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *saveptr: Pointer to the rest of the query
   * @retval None
   */
  void query_select(database_t *db, char *query, char *saveptr);
  /**
   * @brief   Handles the delete query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *saveptr: Pointer to the rest of the query
   * @retval None
   */
  void query_delete(database_t *db, char *query, char *saveptr);
};

#endif
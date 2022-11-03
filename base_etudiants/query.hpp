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
private:
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
   * @param  *query: query that was submitted
   */
  query_result_t(const char *query);
  /**
   * @brief the result of a query.
   **/
  void log_query();
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
   * @param  *p_end_of_query: Pointer to the rest of the query
   * @retval None
   */
  void query_insert(database_t *db, char *query, char *p_end_of_query);
  /**
   * @brief   Handles the update query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *p_end_of_query: Pointer to the rest of the query
   * @retval None
   */
  void query_update(database_t *db, char *query, char *p_end_of_query);
  /**
   * @brief   Handles the select query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *p_end_of_query: Pointer to the rest of the query
   * @retval None
   */
  void query_select(database_t *db, char *query, char *p_end_of_query);
  /**
   * @brief   Handles the delete query
   * @param  *db: database to change
   * @param  *query: Query that was submitted
   * @param  *p_end_of_query: Pointer to the rest of the query
   * @retval None
   */
  void query_delete(database_t *db, char *query, char *p_end_of_query);
  /**
   * @brief  Check if a student has the same value as the field filter
   * @param  *s: the student
   * @param  *field_filter: The field 
   * @param  *value: The value
   * @retval True if the student has the value of field_filter, False if it hasn't
   */
  bool is_student_ok(student_t *s, char *field_filter, char *value);
};

#endif
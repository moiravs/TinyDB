#ifndef _DB_H
#define _DB_H

#include "student.hpp"

/**
 * Database structure type.
 */
struct database_t
{
  student_t *data; /** The list of students **/
  size_t lsize;    /** The logical size of the list **/
  size_t psize;    /** The physical size of the list **/
  int smfd;

  /**
   * @brief  Initialise a database
   * @retval None
   */
  void
  db_init();
  /**
   * @brief  Initialise a database
   * @retval None
   */
  void
  db_init_mem();

  /**
   * @brief  Add a student to the database.
   * @note
   * @param  s: Student to add
   * @retval None
   */
  bool db_add(student_t s);
  /**
   * @brief  Increase size of the database
   * @retval None
   */
  /**
   * @brief  Getter to get db->data[i]
   * @retval None
   */
  student_t *get_record(int i);

  void db_upsize();

  void db_map_memory();

  /**
   * @brief
   * @param  *path: Path of the database to load
   * @retval None
   */
  void
  db_save(const char *path);
  /**
   * @brief Load the content of a database of students.
   * @param  *path: Path of the database to load
   * @retval None
   */
  void db_load(const char *path);
  /**
   * @brief  Delete a student from the database
   * @param  indice: Index of the student to delete
   * @retval None
   */
  void db_delete(size_t indice);
};

#endif
#ifndef _DB_H
#define _DB_H

#include "student.hpp"

/**
 * Database structure type.
 */
class database_t
{
private:
  student_t *data; /** The list of students **/
  size_t lsize;    /** The logical size of the list **/
  size_t psize;    /** The physical size of the list **/
  int smfd;

public:
  /**
   * @brief  Getter of lsize
   * @retval Logical size of database_t
   */
  size_t get_lsize();
  /**
   * @brief  Getter to get db->data[i]
   * @retval A pointer to db->data[i]
   */
  student_t *get_record(int i);
  /**
   * @brief  Initialise a database
   * @retval None
   */
  void db_init();
  /**
   * @brief  Add a student to the database.
   * @param  s: Student to add
   * @retval None
   */
  bool db_add(student_t s);
  /**
   * @brief  Increase size of the database
   * @retval None
   */
  void db_upsize();

  /**
   * @brief Map a file with a memory area
   * @retval None
   */
  void db_map_memory();
  /**
   * @brief Save the contents of the database_t
   * @param  *path: Path of the database to load
   * @retval None
   */
  void db_save(const char *path);
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
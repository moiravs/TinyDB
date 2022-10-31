#ifndef _DB_H
#define _DB_H

#include "student.hpp"

/**
 * Database structure type.
 */
class database_t {
  public :
  student_t *data; /** The list of students **/
  size_t lsize;    /** The logical size of the list **/
  size_t psize;    /** The physical size of the list **/
  /**
   * Initialise a database_t structure.
   * Typical use:
   * ```
   * database_t db;
   * db_init(&db);
   * ```
   **/
  void db_init();
  /**
   *  Add a student to the database.
   **/
  void db_add( student_t s);
  void db_upsize();
      /**
       * Save the content of a database_t in the specified file.
       **/
      void db_save(const char *path);

  /**
   * Load the content of a database of students.
   **/
  void db_load( const char *path);

  void db_resize();

  /**
   * Delete a student from the database
   **/
  void db_delete(size_t indice);
} ;



#endif
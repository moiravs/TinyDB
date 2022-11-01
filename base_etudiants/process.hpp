/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Cursus : BA2-INFO
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/
#include "db.hpp"

extern database_t *db;
extern const char *db_path;
extern int fdInsert[2], fdSelect[2], fdUpdate[2], fdDelete[2], fdResponse[2];

/**
 * @brief  Process who treats the select query
 * @retval None
 */
void process_select();
/**
 * @brief  Process who treats the insert query
 * @retval None
 */
void process_insert();
/**
 * @brief  Process who treats the delete query
 * @retval None
 */
void process_delete();
/**
 * @brief  Process who treats the update query
 * @retval None
 */
void process_update();
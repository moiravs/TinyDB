/*
Projet 2 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Milan
Cursus : BA2-INFO
Description du projet *SmallDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/
#include "db.hpp"

extern database_t *db;
extern const char *db_path;
extern int fd_insert[2], fd_select[2], fd_update[2], fd_delete[2], fd_response[2];
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
/**
 * @brief  Close SmallDB
 * @param  force: False if we need to wait for the queries to terminate
 * @retval None
 */
void close_application(bool force);
/**
 * @brief  Signal Handler
 * @param  signum: Signal Number
 * @retval None
 */
void signal_handling(int signum);
/**
 * @brief  Main Process - Creates children and handles the query
 * @retval None
 */
void main_process();
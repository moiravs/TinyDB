/**
 * Gestion du processus principal.
 **/

#ifndef _MAIN_PROCESS_H
#define _MAIN_PROCESS_H

#include "../db.h"

/**
 * Exécuter le code du processus principal.
 *
 * Crée aussi les processus dédiés.
 **/
void main_process(database_t* db);

#endif

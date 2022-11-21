#ifndef _LOG_H
#define _LOG_H

#include "query.h"

/**
 * Journalise le résultat d'une requête
 * dans un fichier.
 **/
void log_query(query_result_t* result);

#endif

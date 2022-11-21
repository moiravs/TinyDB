#ifndef _PARSING_H
#define _PARSING_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* Required to work with strptime, which is OS-dependent */
#endif

#include "db.h"
#include "query.h"

/**
 * Analyse une requête de type select et l'exécute si
 * elle a été écrite correctement.
 **/
bool perform_select(database_t* db, char* query, query_result_t* result);

/**
 * Analyse une requête de type update et l'exécute si
 * elle a été écrite correctement.
 **/
bool perform_update(database_t* db, char* query, query_result_t* result);

/**
 * Analyse une requête de type insert et l'exécute si
 * elle a été écrite correctement.
 **/
bool perform_insert(database_t* db, char* query, query_result_t* result);

/**
 * Analyse une requête de type delete et l'exécute si
 * elle a été écrite correctement.
 **/
bool perform_delete(database_t* db, char* query, query_result_t* result);

/**
 * Teste si la requête est de type select.
 **/
inline bool is_select_request(const char* query) {
   return strncmp(query, "select ", sizeof("select ") - 1) == 0;
}

/**
 * Teste si la requête est de type select.
 **/
inline bool is_update_request(const char* query) {
   return strncmp(query, "update ", sizeof("update ") - 1) == 0;
}

/**
 * Teste si la requête est de type insert.
 **/
inline bool is_insert_request(const char* query) {
   return strncmp(query, "insert ", sizeof("insert ") - 1) == 0;
}

/**
 * Teste si la requête est de type delete.
 **/
inline bool is_delete_request(const char* query) {
   return strncmp(query, "delete ", sizeof("delete ") - 1) == 0;
}

/**
 * Teste si la requête est de type transaction.
 **/
inline bool is_transaction_request(const char* query) {
   return strncmp(query, "transaction\n", sizeof("transaction\n") - 1) == 0;
}

#endif
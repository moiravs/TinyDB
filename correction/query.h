#ifndef _QUERY_H
#define _QUERY_H

#include "db.h"

#define QUERY_MAX_SIZE 512

typedef enum {
    QUERY_SUCCESS,
    QUERY_FAILURE,
    UNRECOGNISED_FIELD
} QUERY_STATUS;

typedef struct {
    student_t* students; /** Liste d'étudiants concernés par la requête **/
    size_t lsize;        /** Taille logique (nombre d'étudiants réellement là) **/
    size_t psize;        /** Taille physique (nombre maximum d'étudiants possible) **/
    QUERY_STATUS status; /** Le statut de la requête **/
    char query[QUERY_MAX_SIZE]; /** La requête ayant donné ce résultat **/
    long start_ns;       /** Début de la requête en nanosecondes **/
    long end_ns;         /** Fin de la requête en nanosecondes **/
    unsigned* indices;   /** Indices des étudiants dans la BDD **/
} query_result_t;

/**
 * Initialise a query_result_t structure.
 **/
void query_result_init(query_result_t* result, const char* query);

/**
 * Free a query result.
 **/
void query_result_free_internals(query_result_t* result);

/**
 * Add a student to a query result.
 **/
void query_result_add(query_result_t* result, student_t s, unsigned idx);

/**
 * Handle select queries
 **/
void query_select(database_t* db, const char* field, const char* value, query_result_t* query_result);

/**
 * Handle delete queries.
 **/
void query_delete(database_t* db, char* field, char* value, query_result_t* result);

/**
 * Perform an insert query.
 **/
void query_insert(database_t* db, char* fname, char* lname, unsigned id, char* section, struct tm birthdate, query_result_t* result);

/**
 * Handle update queries
 **/
void query_update(database_t* db, char* field_filter, char* value_filter, char* field_to_update, char* update_value, query_result_t* result);
#endif

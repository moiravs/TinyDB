#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "db.h"
#include "log.h"
#include "parsing.h"
#include "query.h"

void query_result_init(query_result_t* result, const char* query) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
    result->status = QUERY_SUCCESS;
    result->lsize = 0;
    result->psize = 100;
    result->students = (student_t*)malloc(sizeof(student_t) * result->psize);
    result->indices = (unsigned*)malloc(sizeof(int) * result->psize);
    strcpy(result->query, query);
    if (result->query[0] != '\0')
       result->query[strlen(result->query) - 1] = '\0';
}

void query_result_free_internals(query_result_t* result) {
    free(result->students);
    free(result->indices);
}

void query_result_add(query_result_t* result, student_t s, unsigned idx) {
    if (result->lsize >= result->psize) {
        result->psize = result->psize * 2;
        result->students = (student_t*)realloc(result->students, sizeof(student_t) * result->psize);
        result->indices = (unsigned*)realloc(result->indices, sizeof(int) * result->psize);
    }
    result->students[result->lsize] = s;
    result->indices[result->lsize] = idx;
    result->lsize++;
}

void query_select(database_t* db, const char* field, const char* value, query_result_t* query_result) {
    if (strcmp(field, "fname") == 0) {
        for (size_t i = 0; i < *db->lsize; i++) {
            student_t s = db->data[i];
            if (strcmp(value, s.fname) == 0) {
                query_result_add(query_result, s, i);
            }
        }
    } else if (strcmp(field, "lname") == 0) {
        for (size_t i = 0; i < *db->lsize; i++) {
            student_t s = db->data[i];
            if (strcmp(value, s.lname) == 0) {
                query_result_add(query_result, s, i);
            }
        }
    } else if (strcmp(field, "birthdate") == 0) {
        struct tm date;
        strptime(value, "%d/%m/%Y", &date);
        for (size_t i = 0; i < *db->lsize; i++) {
            student_t s = db->data[i];
            if (s.birthdate.tm_year == date.tm_year && s.birthdate.tm_mon == date.tm_mon && s.birthdate.tm_mday == date.tm_mday) {
                query_result_add(query_result, s, i);
            }
        }
    } else if (strcmp(field, "section") == 0) {
        for (size_t i = 0; i < *db->lsize; i++) {
            student_t s = db->data[i];
            if (strcmp(s.section, value) == 0) {
                query_result_add(query_result, s, i);
            }
        }
    } else if (strcmp(field, "id") == 0) {
        unsigned id = atol(value);
        for (size_t i = 0; i < *db->lsize; i++) {
            student_t s = db->data[i];
            if (s.id == id) {
                query_result_add(query_result, s, i);
            }
        }
    } else {
        query_result->status = UNRECOGNISED_FIELD;
    }
}

void query_delete(database_t* db, char* field, char* value, query_result_t* result) {
    query_select(db, field, value, result);
    for (size_t i = 0; i < result->lsize; i++) {
        db_delete(db, &result->students[i]);
    }
}

/**
 * Vérifie si l'ID d'étudiant donné existe dans
 * la base de données.
 **/
static inline bool is_id_existing(database_t* db, unsigned id) {
   query_result_t query_unique_id;
   char id_str[50];

   sprintf(id_str, "%u", id);
   query_result_init(&query_unique_id, "");
   query_select(db, "id", id_str, &query_unique_id);
   
   query_result_free_internals(&query_unique_id);
   
   return query_unique_id.lsize == 0;
}

void query_insert(database_t* db, char* fname, char* lname, unsigned id, char* section, struct tm birthdate, query_result_t* result) {
    student_t s;
    strcpy(s.lname, lname);
    strcpy(s.fname, fname);
    strcpy(s.section, section);
    s.birthdate = birthdate;
    s.id = id;
    
    if (is_id_existing(db, id)) {
       query_result_add(result, s, result->lsize);
       db_add(db, s);
    } else {
       result->status = QUERY_FAILURE;
    }
}

void query_update(database_t* db, char* field_filter, char* value_filter, char* field_to_update, char* update_value, query_result_t* result) {
    query_select(db, field_filter, value_filter, result);
    
    if (result->status == UNRECOGNISED_FIELD) {
       return;
    }

    // Update the values for the matches
    for (size_t i = 0; i < result->lsize; i++) {
        unsigned idx = result->indices[i];
        if (strcmp(field_to_update, "id") == 0) {
            db->data[idx].id = (unsigned)atoi(update_value);
            result->students[i].id = db->data[idx].id;
        } else if (strcmp(field_to_update, "fname") == 0) {
            strcpy(db->data[idx].fname, update_value);
            strcpy(result->students[i].fname, update_value);
        } else if (strcmp(field_to_update, "lname") == 0) {
            strcpy(db->data[idx].lname, update_value);
            strcpy(result->students[i].lname, update_value);
        } else if (strcmp(field_to_update, "birthdate") == 0) {
            strptime(update_value, "%d/%m/%Y", &db->data[idx].birthdate);
            result->students[i].birthdate = db->data[idx].birthdate;
        } else if (strcmp(field_to_update, "section") == 0) {
            strcpy(db->data[idx].section, update_value);
            strcpy(result->students[i].section, update_value);
        } else {
            result->status = UNRECOGNISED_FIELD;
            return;
        }
    }
}

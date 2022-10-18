#include "parsing.hpp"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

#include "student.hpp"

bool parse_update(char* query, char* field_filter, char* value_filter, char* field_to_update, char* update_value) {
    char* key_val_filter = strtok_r(NULL, " ", &query);  // key=val filter
    if (key_val_filter == NULL) {
        return false;
    }
    if (strtok_r(NULL, " ", &query) == NULL) {  // discard the "set" keyword
        return false;
    }

    char* key_val_update = strtok_r(NULL, " ", &query);  // key=val updated value
    if (key_val_update == NULL) {
        return false;
    }

    if (parse_selectors(key_val_filter, field_filter, value_filter) == 0) {
        return false;
    }
    if (parse_selectors(key_val_update, field_to_update, update_value) == 0) {
        return false;
    }
    return true;
}

bool parse_insert(char* query, char* fname, char* lname, unsigned* id, char* section, struct tm* birthdate) {
    std::cout << "jpasse par là ush" << query;
    char* token = strtok_r(NULL, " ", &query);
    
    if (token == NULL) {
        return false;
    }
    strcpy(fname, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(lname, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    *id = (unsigned)atol(token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(section, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    if (strptime(token, "%d/%m/%Y", birthdate) == NULL) {
        return false;
    }
    return true;
}

bool parse_selectors(char* query, char* field, char* value) {
    char* token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(field, token);
    token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(value, token);
    return true;
}

// TODO bool parse_delete
// delete < champ >= <valeur>.Cette requête supprime tous les étudiants qui correspondent au filtre donné.
bool parse_delete(){
    return false;
}
// TODO bool parse_transaction
bool parse_transaction()
{
    return false;
}
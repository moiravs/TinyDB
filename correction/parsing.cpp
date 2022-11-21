#include <cstdlib>
#include <cstring>
#include <ctime>

#include "parsing.h"
#include "db.h"
#include "query.h"

bool perform_select(database_t* db, char* query, query_result_t* result) {
   char field[32], value[64];
   if (sscanf(query, "select %31[^=]=%63s\n", field, value) != 2) {
      fprintf(stderr, "The select query is not well written.");
      return 0;
   }
   
   query_select(db, field, value, result);
   return 1;
}

bool perform_update(database_t* db, char* query, query_result_t* result) {
   char field_filter[32], value_filter[64], field_to_update[32], update_value[64];
         
   if (sscanf(query, "update %31[^=]=%63s set %31[^=]=%63s\n", field_filter, value_filter, field_to_update, update_value) != 4) {
      fprintf(stderr, "The update query is not well written.\n");
      return 0;
   }
   
   query_update(db, field_filter, value_filter, field_to_update, update_value, result);
   return 1;
}

bool perform_insert(database_t* db, char* query, query_result_t* result) {
   char fname[64], lname[64], section[64], date[11];
   unsigned id;
   struct tm birthdate;
   
   if (sscanf(query, "insert %63s %63s %u %63s %10s\n", fname, lname, &id, section, date) != 5
       || strptime(date, "%d/%m/%Y", &birthdate) == NULL) {
      fprintf(stderr, "The insert query is not well written.\n");
      return 0;
   }
   
   query_insert(db, fname, lname, id, section, birthdate, result);
   return 1;
}

bool perform_delete(database_t* db, char* query, query_result_t* result) {
   char field[32], value[64];
   if (sscanf(query, "delete %31[^=]=%63s\n", field, value) != 2) {
      fprintf(stderr, "The delete query is not well written.s\n");
      return 0;
   }
   
   query_delete(db, field, value, result);
   return 1;
}
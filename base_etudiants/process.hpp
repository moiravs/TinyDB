#include "db.hpp"

extern database_t *db;
extern const char *db_path;
extern int fdInsert[2], fdSelect[2], fdUpdate[2], fdDelete[2], fdResponse[2];

void process_select();
void process_insert();
void process_delete();
void process_update();
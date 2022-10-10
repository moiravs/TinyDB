#include <stdio.h>

#include "db.h"

int main(int argc, char const *argv[]) {
  const char *db_path = "";
  database_t db;
  db_init(&db);
  db_load(&db, db_path);
  // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  printf("Bye bye!\n");
  return 0;
}
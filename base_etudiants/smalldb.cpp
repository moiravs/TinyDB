#include "db.hpp"

int main(int argc, char const *argv[])
{

  database_t *db = new database_t;
  db->path = argv[1];
  db_load(db, db->path);
  return 0;
}

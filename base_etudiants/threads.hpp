#include "db.hpp"


struct pipes
{
    int *filed1;
    int *filed2;
    int *filed3;
    int *filed4;
    database_t * db;
};

void *fonction_1(void *fd);
void *fonction_2(void *fd);
void *fonction_3(void *fd);
void *fonction_4(void *fd);
void creation_thread_and_pipes(struct pipes *args);
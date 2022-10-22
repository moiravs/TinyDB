#include "db.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <iostream>
#include "student.hpp"
#include <sys/mman.h>

void db_save(database_t *db, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(db->data, sizeof(student_t), db->lsize, f) < 0)
    {
        perror("Could not write in the DB file");
        exit(1);
    }
    std::cout << "i wrote";
    fclose(f);
}

void db_load(database_t *db, const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file))
    {
        db_add(db, student);
    }
    fclose(file);
}

void db_upsize(database_t *db)
{
    puts("here");
    if (db->lsize >= (db->psize / sizeof(student_t)))
    {
        puts("bfrfrfr");
        size_t old_psize = db->psize;
        db->psize = db->psize * 2;
        db->data = (student_t *)mremap(db->data, old_psize, db->psize, MREMAP_MAYMOVE);
    }
}

void db_init(database_t *db)
{
    db->lsize = 0;
    db->psize = sizeof(student_t) * 16;
    db->data = (student_t *)mmap(NULL, db->psize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}

void db_add(database_t *db, student_t student)
{
    db->lsize += 1;
    db_upsize(db);
    db->data[db->lsize] = student; // at end of db
}

void db_delete(database_t *db, size_t indice)
{
    if (indice >= db->lsize)
    { // indice out of range
        perror("db_delete()");
    }
    db->lsize--;
    for (size_t i = indice; i < db->lsize; i++)
    {
        db->data[i] = db->data[i + 1];
    }
}